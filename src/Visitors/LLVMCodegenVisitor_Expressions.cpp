#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/LetExprNode.hpp"
#include "Expressions/AssignmentNode.hpp"
#include "Expressions/MemberAssignmentNode.hpp"
#include "Expressions/SelfMemberAssignmentNode.hpp"
#include "Expressions/BlockExprNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <iostream>


extern std::map<std::string, llvm::StructType*> structTypes;


static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(LetExprNode& node) {

    localVarsStack.push_back(localVarsStack.empty() ? std::map<std::string, llvm::AllocaInst*>() : localVarsStack.back());


    for (const auto& decl : node.getDeclarations()) {
        decl.expr->accept(*this);
        llvm::Value* val = lastValue;


        if (auto typeInst = dynamic_cast<TypeInstantiationNode*>(decl.expr)) {
            variableTypes[decl.id] = typeInst->typeName;
        }

        llvm::Function* func = builder.GetInsertBlock()->getParent();
        llvm::AllocaInst* alloca = createEntryBlockAlloca(func, val->getType(), decl.id);
        builder.CreateStore(val, alloca);
        localVarsStack.back()[decl.id] = alloca;
    }


    node.getBody()->accept(*this);
    llvm::Value* result = lastValue; 


    localVarsStack.pop_back();


    lastValue = result;
}

void LLVMCodegenVisitor::visit(AssignmentNode& node) {

    node.value->accept(*this);
    llvm::Value* val = lastValue;


    llvm::AllocaInst* alloca = nullptr;
    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto found = it->find(node.identifier);
        if (found != it->end()) {
            alloca = found->second;
            break;
        }
    }

    if (alloca) {

        builder.CreateStore(val, alloca);

        lastValue = val;
    } else {

        lastValue = nullptr;
    }
}

void LLVMCodegenVisitor::visit(MemberAssignmentNode& node) {


    node.value->accept(*this);
    llvm::Value* val = lastValue;
    

    lastValue = val;
}

void LLVMCodegenVisitor::visit(SelfMemberAssignmentNode& node) {

    node.value->accept(*this);
    llvm::Value* val = lastValue;
    
    if (!val) {
        lastValue = nullptr;
        return;
    }
    

    llvm::Value* selfPtr = nullptr;
    

    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto selfIt = it->find("self");
        if (selfIt != it->end()) {
            selfPtr = builder.CreateLoad(selfIt->second->getAllocatedType(), selfIt->second, "self");
            break;
        }
    }
    
    if (!selfPtr) {
        std::cerr << "Error: 'self' not found in current context for member assignment" << std::endl;
        lastValue = nullptr;
        return;
    }
    

    std::string selfTypeName = currentTypeName.empty() ? "Object" : currentTypeName;
    

    auto typeIt = types.find(selfTypeName);
    if (typeIt != types.end()) {
        TypeDefNode* typeDef = typeIt->second;
        

        int attrIndex = -1;
        size_t currentIndex = 0;
        

        if (!typeDef->parentTypeName.empty()) {
            auto parentTypeIt = types.find(typeDef->parentTypeName);
            if (parentTypeIt != types.end()) {
                TypeDefNode* parentTypeDef = parentTypeIt->second;
                for (size_t i = 0; i < parentTypeDef->attributes.size(); ++i) {
                    if (parentTypeDef->attributes[i].name == node.member) {
                        attrIndex = currentIndex;
                        break;
                    }
                    currentIndex++;
                }
            }
        }
        

        if (attrIndex == -1) {
            for (size_t i = 0; i < typeDef->attributes.size(); ++i) {
                if (typeDef->attributes[i].name == node.member) {
                    attrIndex = currentIndex;
                    break;
                }
                currentIndex++;
            }
        }
        
        if (attrIndex >= 0) {

            auto structIt = structTypes.find(selfTypeName);
            if (structIt != structTypes.end()) {
                llvm::StructType* structType = structIt->second;
                

                llvm::Value* typedSelfPtr = builder.CreateBitCast(selfPtr, llvm::PointerType::get(structType, 0), "typed_self");
                

                llvm::Value* memberPtr = builder.CreateStructGEP(structType, typedSelfPtr, attrIndex, "member_ptr");
                

                // Convert value to the correct type if needed
                llvm::Type* memberType = structType->getElementType(attrIndex);
                llvm::Value* convertedVal = val;
                
                if (val->getType() != memberType) {
                    if (val->getType()->isDoubleTy() && memberType->isIntegerTy(32)) {
                        // Convert double to i32
                        convertedVal = builder.CreateFPToSI(val, memberType, "double_to_int");
                    } else if (val->getType()->isIntegerTy(32) && memberType->isDoubleTy()) {
                        // Convert i32 to double
                        convertedVal = builder.CreateSIToFP(val, memberType, "int_to_double");
                    }
                }
                
                builder.CreateStore(convertedVal, memberPtr);
                
                std::cerr << "DEBUG: Successfully assigned value to member '" << node.member << "' at index " << attrIndex << std::endl;
                

                lastValue = val;
            } else {
                std::cerr << "Error: Could not find struct type for " << selfTypeName << std::endl;
                lastValue = nullptr;
            }
        } else {
            std::cerr << "Error: Member '" << node.member << "' not found in type '" << selfTypeName << "'" << std::endl;
            lastValue = nullptr;
        }
    } else {
        std::cerr << "Error: Type definition not found for self (selfTypeName='" << selfTypeName << "')" << std::endl;
        lastValue = nullptr;
    }
}

void LLVMCodegenVisitor::visit(BlockExprNode& node) {

    llvm::Value* blockResult = nullptr;
    
    for (auto expr : node.expressions) {
        expr->accept(*this);

        if (lastValue) {
            blockResult = lastValue;
        }
    }
    

    lastValue = blockResult;
}