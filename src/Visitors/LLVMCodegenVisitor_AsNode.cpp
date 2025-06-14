#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/AsNode.hpp"
#include "Expressions/IsNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <iostream>

void LLVMCodegenVisitor::visit(AsNode& node) {

    node.expression->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    if (!exprValue) {
        std::cerr << "Error: Invalid expression in 'as' cast" << std::endl;
        lastValue = nullptr;
        return;
    }
    

    std::string exprTypeName = "";
    

    if (auto varNode = dynamic_cast<VariableNode*>(node.expression)) {
        auto typeIt = variableTypes.find(varNode->identifier);
        if (typeIt != variableTypes.end()) {
            exprTypeName = typeIt->second;
        }
    }

    else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.expression)) {
        exprTypeName = typeInstNode->typeName;
    }

    else {
        llvm::Type* llvmType = exprValue->getType();
        if (llvmType->isIntegerTy(32)) {
            exprTypeName = "Number";
        } else if (llvmType->isIntegerTy(1)) {
            exprTypeName = "Boolean";
        } else if (llvmType->isPointerTy()) {


            exprTypeName = "String";
        }
    }
    

    bool canCast = false;
    
    if (exprTypeName == node.typeName) {

        canCast = true;
    } else if (!exprTypeName.empty()) {

        auto typeIt = types.find(exprTypeName);
        if (typeIt != types.end()) {
            TypeDefNode* currentType = typeIt->second;
            

            std::string currentParentName = currentType->parentTypeName;
            while (!currentParentName.empty() && !canCast) {
                if (currentParentName == node.typeName) {
                    canCast = true;
                    break;
                }
                
                auto parentTypeIt = types.find(currentParentName);
                if (parentTypeIt != types.end()) {
                    currentParentName = parentTypeIt->second->parentTypeName;
                } else {
                    break;
                }
            }
        }
        

        if (!canCast) {
            auto targetTypeIt = types.find(node.typeName);
            if (targetTypeIt != types.end()) {
                TypeDefNode* targetType = targetTypeIt->second;
                
                std::string currentParentName = targetType->parentTypeName;
                while (!currentParentName.empty() && !canCast) {
                    if (currentParentName == exprTypeName) {
                        canCast = true;
                        break;
                    }
                    
                    auto parentTypeIt = types.find(currentParentName);
                    if (parentTypeIt != types.end()) {
                        currentParentName = parentTypeIt->second->parentTypeName;
                    } else {
                        break;
                    }
                }
            }
        }
    }
    

    if (!canCast && exprTypeName.empty()) {

        llvm::Type* llvmType = exprValue->getType();
        
        if (node.typeName == "Number" && llvmType->isIntegerTy(32)) {
            canCast = true;
        } else if (node.typeName == "Boolean" && llvmType->isIntegerTy(1)) {
            canCast = true;
        } else if (node.typeName == "String" && llvmType->isPointerTy()) {
            canCast = true;
        } else if (node.typeName == "Object") {

            canCast = true;
        }
    }
    

    if (canCast && !exprTypeName.empty() && 
        (exprTypeName == "Number" || exprTypeName == "Boolean" || exprTypeName == "String") &&
        (node.typeName == "Number" || node.typeName == "Boolean" || node.typeName == "String")) {
        
        llvm::Type* exprType = exprValue->getType();
        llvm::Type* targetType = nullptr;
        
        if (node.typeName == "Number") {
            targetType = llvm::Type::getInt32Ty(ctx);
        } else if (node.typeName == "Boolean") {
            targetType = llvm::Type::getInt1Ty(ctx);
        } else if (node.typeName == "String") {
            targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
        }
        
        if (targetType && exprType != targetType) {
            if (exprType->isIntegerTy() && targetType->isIntegerTy()) {
                if (exprType->getIntegerBitWidth() < targetType->getIntegerBitWidth()) {
                    lastValue = builder.CreateZExt(exprValue, targetType, "as_int_extend");
                } else if (exprType->getIntegerBitWidth() > targetType->getIntegerBitWidth()) {
                    lastValue = builder.CreateTrunc(exprValue, targetType, "as_int_trunc");
                } else {
                    lastValue = exprValue;
                }
            } else if (exprType->isIntegerTy(1) && targetType->isIntegerTy()) {
                lastValue = builder.CreateZExt(exprValue, targetType, "as_bool_to_int");
            } else if (exprType->isIntegerTy() && targetType->isIntegerTy(1)) {
                llvm::Value* zero = llvm::ConstantInt::get(exprType, 0);
                lastValue = builder.CreateICmpNE(exprValue, zero, "as_int_to_bool");
            } else if (exprType->isPointerTy() && targetType->isPointerTy()) {
                lastValue = builder.CreateBitCast(exprValue, targetType, "as_ptr_cast");
            } else {
                lastValue = exprValue;
            }
        } else {
            lastValue = exprValue;
        }
    } else if (canCast) {

        llvm::Type* targetType = nullptr;
        if (node.typeName == "Number") {
            targetType = llvm::Type::getInt32Ty(ctx);
        } else if (node.typeName == "Boolean") {
            targetType = llvm::Type::getInt1Ty(ctx);
        } else if (node.typeName == "String") {
            targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
        } else {
            targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
        }
        
        if (targetType && exprValue->getType() != targetType) {
            if (exprValue->getType()->isPointerTy() && targetType->isPointerTy()) {
                lastValue = builder.CreateBitCast(exprValue, targetType, "as_custom_cast");
            } else {
                lastValue = exprValue;
            }
        } else {
            lastValue = exprValue;
        }
    } else {

        llvm::Function* printfFunc = module.getFunction("printf");
        if (!printfFunc) {
            llvm::FunctionType* printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx),
                {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
                true
            );
            printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);
        }
        
        llvm::Function* exitFunc = module.getFunction("exit");
        if (!exitFunc) {
            llvm::FunctionType* exitType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(ctx),
                {llvm::Type::getInt32Ty(ctx)},
                false
            );
            exitFunc = llvm::Function::Create(exitType, llvm::Function::ExternalLinkage, "exit", module);
        }
        

        std::string errorMsg = "Runtime error: Cannot cast " + exprTypeName + " to " + node.typeName + "\\n";
        llvm::Value* errorStr = builder.CreateGlobalStringPtr(errorMsg, "cast_error_msg");
        

        builder.CreateCall(printfFunc, {errorStr});
        builder.CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
        

        lastValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
    }
    
    std::cerr << "DEBUG: AsNode - Expression type: '" << exprTypeName 
              << "', Target type: '" << node.typeName 
              << "', Can cast: " << (canCast ? "true" : "false") << std::endl;
}