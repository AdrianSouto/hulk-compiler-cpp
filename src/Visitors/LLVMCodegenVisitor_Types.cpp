#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/SelfMemberAccessNode.hpp"
#include "Expressions/MemberAccessNode.hpp"
#include "Expressions/MethodCallNode.hpp"
#include "Expressions/IsNode.hpp"
#include "Expressions/AsNode.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <iostream>


void LLVMCodegenVisitor::visit(TypeDefNode& node) {
    
    std::vector<llvm::Type*> memberTypes;
    
    
    for (const auto& attr : node.attributes) {
        llvm::Type* attrType = nullptr;
        
        
        if (attr.type && attr.type->toString() == "Number") {
            attrType = llvm::Type::getDoubleTy(ctx);
        } else if (attr.type && attr.type->toString() == "String") {
            attrType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
        } else if (attr.type && attr.type->toString() == "Boolean") {
            attrType = llvm::Type::getInt1Ty(ctx);
        } else {
            
            attrType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0); 
        }
        
        memberTypes.push_back(attrType);
    }
    
    
    llvm::StructType* structType = llvm::StructType::create(ctx, memberTypes, node.typeName);
    
    
    
    
    
    if (!node.typeArguments.empty()) {
        std::vector<llvm::Type*> paramTypes;
        
        
        for (const auto& param : node.typeArguments) {
            if (param.type && param.type->toString() == "Number") {
                paramTypes.push_back(llvm::Type::getDoubleTy(ctx));
            } else if (param.type && param.type->toString() == "String") {
                paramTypes.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
            } else if (param.type && param.type->toString() == "Boolean") {
                paramTypes.push_back(llvm::Type::getInt1Ty(ctx));
            } else {
                paramTypes.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
            }
        }
        
        
        llvm::Type* returnType = llvm::PointerType::get(structType, 0);
        
        
        llvm::FunctionType* constructorType = llvm::FunctionType::get(returnType, paramTypes, false);
        
        
        std::string constructorName = node.typeName + "_constructor";
        llvm::Function* constructor = llvm::Function::Create(
            constructorType, 
            llvm::Function::ExternalLinkage, 
            constructorName, 
            &module
        );
        
        
        llvm::BasicBlock* constructorBB = llvm::BasicBlock::Create(ctx, "entry", constructor);
        builder.SetInsertPoint(constructorBB);
        
        
        llvm::Value* instance = builder.CreateAlloca(structType, nullptr, "instance");
        
        
        auto argIt = constructor->arg_begin();
        for (size_t i = 0; i < node.attributes.size() && i < node.typeArguments.size(); ++i) {
            llvm::Value* fieldPtr = builder.CreateStructGEP(structType, instance, i);
            builder.CreateStore(&*argIt, fieldPtr);
            ++argIt;
        }
        
        
        builder.CreateRet(instance);
    }
    
    lastValue = nullptr; 
}


void LLVMCodegenVisitor::visit(TypeInstantiationNode& node) {
    
    std::string constructorName = node.typeName + "_constructor";
    llvm::Function* constructor = module.getFunction(constructorName);
    
    if (!constructor) {
        std::cerr << "Error: Constructor not found for type " << node.typeName << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
    std::vector<llvm::Value*> args;
    for (auto* arg : node.arguments) {
        arg->accept(*this);
        if (lastValue) {
            args.push_back(lastValue);
        }
    }
    
    
    lastValue = builder.CreateCall(constructor, args, "instance");
}


void LLVMCodegenVisitor::visit(SelfMemberAccessNode& node) {
    
    
    
    llvm::AllocaInst* selfPtr = nullptr;
    
    
    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto selfIt = it->find("self");
        if (selfIt != it->end()) {
            selfPtr = selfIt->second;
            break;
        }
    }
    
    if (!selfPtr) {
        std::cerr << "Error: 'self' not found in current context" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
    llvm::Value* selfValue = builder.CreateLoad(selfPtr->getAllocatedType(), selfPtr, "self");
    
    
    
    
    
    
    
    int fieldIndex = 0; 
    
    
    llvm::Type* selfType = selfPtr->getAllocatedType();
    
    
    llvm::Value* fieldPtr = builder.CreateStructGEP(
        selfType, 
        selfValue, 
        fieldIndex, 
        node.attributeName
    );
    
    
    llvm::Type* fieldType = llvm::cast<llvm::StructType>(selfType)->getElementType(fieldIndex);
    lastValue = builder.CreateLoad(fieldType, fieldPtr, node.attributeName);
}


void LLVMCodegenVisitor::visit(MemberAccessNode& node) {
    
    node.object->accept(*this);
    llvm::Value* objectValue = lastValue;
    
    if (!objectValue) {
        std::cerr << "Error: Invalid object in member access" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
    
    std::cerr << "Warning: MemberAccessNode visitor needs proper type tracking implementation" << std::endl;
    lastValue = nullptr; 
}


void LLVMCodegenVisitor::visit(MethodCallNode& node) {
    
    node.object->accept(*this);
    llvm::Value* objectValue = lastValue;
    
    if (!objectValue) {
        std::cerr << "Error: Invalid object in method call" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
    
    std::string methodName = node.methodName; 
    llvm::Function* method = module.getFunction(methodName);
    
    if (!method) {
        std::cerr << "Error: Method " << node.methodName << " not found" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
    std::vector<llvm::Value*> args;
    args.push_back(objectValue);
    
    
    for (auto* arg : node.arguments) {
        arg->accept(*this);
        if (lastValue) {
            args.push_back(lastValue);
        }
    }
    
    
    lastValue = builder.CreateCall(method, args, "method_result");
}


void LLVMCodegenVisitor::visit(IsNode& node) {
    
    node.expression->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    if (!exprValue) {
        std::cerr << "Error: Invalid expression in 'is' check" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
    llvm::Type* exprType = exprValue->getType();
    bool isMatch = false;
    
    if (node.typeName == "Number") {
        isMatch = exprType->isDoubleTy() || exprType->isFloatTy() || exprType->isIntegerTy();
    } else if (node.typeName == "String") {
        isMatch = exprType->isPointerTy();
        
        
    } else if (node.typeName == "Boolean") {
        isMatch = exprType->isIntegerTy(1);
    } else {
        
        
        isMatch = false;
    }
    
    
    lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), isMatch ? 1 : 0);
}


void LLVMCodegenVisitor::visit(AsNode& node) {
    
    node.expression->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    if (!exprValue) {
        std::cerr << "Error: Invalid expression in 'as' cast" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    llvm::Type* exprType = exprValue->getType();
    llvm::Type* targetType = nullptr;
    
    
    if (node.typeName == "Number") {
        targetType = llvm::Type::getDoubleTy(ctx);
    } else if (node.typeName == "String") {
        targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    } else if (node.typeName == "Boolean") {
        targetType = llvm::Type::getInt1Ty(ctx);
    } else {
        
        targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    }
    
    
    if (exprType == targetType) {
        
        lastValue = exprValue;
    } else if (exprType->isIntegerTy() && targetType->isDoubleTy()) {
        
        lastValue = builder.CreateSIToFP(exprValue, targetType, "int_to_double");
    } else if (exprType->isDoubleTy() && targetType->isIntegerTy()) {
        
        lastValue = builder.CreateFPToSI(exprValue, targetType, "double_to_int");
    } else if (exprType->isPointerTy() && targetType->isPointerTy()) {
        
        lastValue = builder.CreateBitCast(exprValue, targetType, "ptr_cast");
    } else {
        
        std::cerr << "Warning: Unsupported cast in 'as' operation" << std::endl;
        lastValue = exprValue; 
    }
}