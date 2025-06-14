#include "Visitors/LLVMCodegenVisitor.hpp"
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
#include <iostream>

void LLVMCodegenVisitor::visit(IsNode& node) {

    node.expression->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    if (!exprValue) {
        std::cerr << "Error: Invalid expression in 'is' check" << std::endl;
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), 0);
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
    

    bool isMatch = false;
    
    if (exprTypeName == node.typeName) {

        isMatch = true;
    } else if (!exprTypeName.empty()) {

        auto typeIt = types.find(exprTypeName);
        if (typeIt != types.end()) {
            TypeDefNode* currentType = typeIt->second;
            

            std::string currentParentName = currentType->parentTypeName;
            while (!currentParentName.empty() && !isMatch) {
                if (currentParentName == node.typeName) {
                    isMatch = true;
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
        

        if (!isMatch) {
            auto targetTypeIt = types.find(node.typeName);
            if (targetTypeIt != types.end()) {
                TypeDefNode* targetType = targetTypeIt->second;
                
                std::string currentParentName = targetType->parentTypeName;
                while (!currentParentName.empty() && !isMatch) {
                    if (currentParentName == exprTypeName) {
                        isMatch = true;
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
    

    if (!isMatch && exprTypeName.empty()) {

        llvm::Type* llvmType = exprValue->getType();
        
        if (node.typeName == "Number" && llvmType->isIntegerTy(32)) {
            isMatch = true;
        } else if (node.typeName == "Boolean" && llvmType->isIntegerTy(1)) {
            isMatch = true;
        } else if (node.typeName == "String" && llvmType->isPointerTy()) {

            isMatch = true;
        } else if (node.typeName == "Object") {

            isMatch = true;
        }
    }
    

    lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), isMatch ? 1 : 0);
    
    std::cerr << "DEBUG: IsNode - Expression type: '" << exprTypeName 
              << "', Target type: '" << node.typeName 
              << "', Result: " << (isMatch ? "true" : "false") << std::endl;
}