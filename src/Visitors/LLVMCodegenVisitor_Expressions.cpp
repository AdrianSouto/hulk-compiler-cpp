#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/LetExprNode.hpp"
#include "Expressions/AssignmentNode.hpp"
#include "Expressions/MemberAssignmentNode.hpp"
#include "Expressions/SelfMemberAssignmentNode.hpp"
#include "Expressions/BlockExprNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/MemberAccessNode.hpp"
#include "Expressions/SelfMemberAccessNode.hpp"
#include "Expressions/MethodCallNode.hpp"
#include "Expressions/VariableNode.hpp"
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


        if (decl.type) {
            // Use declared type if available
            variableTypes[decl.id] = decl.type->getTypeName();
        } else if (auto typeInst = dynamic_cast<TypeInstantiationNode*>(decl.expr)) {
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

void LLVMCodegenVisitor::visit(MemberAccessNode& node) {
    // Evaluate the object expression
    node.object->accept(*this);
    llvm::Value* objPtr = lastValue;
    
    if (!objPtr) {
        std::cerr << "Error: Invalid object in member access" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    // For now, implement basic member access
    // This would need to be enhanced based on the object type and member
    std::cerr << "DEBUG: Member access to '" << node.memberName << "'" << std::endl;
    
    // TODO: Implement proper member access based on object type
    lastValue = objPtr; // Placeholder
}

void LLVMCodegenVisitor::visit(SelfMemberAccessNode& node) {
    // Find 'self' in the current context
    llvm::Value* selfPtr = nullptr;
    
    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto selfIt = it->find("self");
        if (selfIt != it->end()) {
            selfPtr = builder.CreateLoad(selfIt->second->getAllocatedType(), selfIt->second, "self");
            break;
        }
    }
    
    if (!selfPtr) {
        std::cerr << "Error: 'self' not found in current context for member access" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    // Get the current type name
    std::string selfTypeName = currentTypeName.empty() ? "Object" : currentTypeName;
    
    // Find the attribute in the type definition
    auto typeIt = types.find(selfTypeName);
    if (typeIt != types.end()) {
        TypeDefNode* typeDef = typeIt->second;
        
        int attrIndex = -1;
        size_t currentIndex = 1; // Start from 1 to skip TypeInfo* field
        
        // Check inherited attributes first
        if (!typeDef->parentTypeName.empty()) {
            auto parentTypeIt = types.find(typeDef->parentTypeName);
            if (parentTypeIt != types.end()) {
                TypeDefNode* parentTypeDef = parentTypeIt->second;
                for (size_t i = 0; i < parentTypeDef->attributes.size(); ++i) {
                    if (parentTypeDef->attributes[i].name == node.attributeName) {
                        attrIndex = currentIndex;
                        break;
                    }
                    currentIndex++;
                }
            }
        }
        
        // Check own attributes
        if (attrIndex == -1) {
            for (size_t i = 0; i < typeDef->attributes.size(); ++i) {
                if (typeDef->attributes[i].name == node.attributeName) {
                    attrIndex = currentIndex;
                    break;
                }
                currentIndex++;
            }
        }
        
        if (attrIndex >= 0) {
            // Get the struct type
            auto structIt = structTypes.find(selfTypeName);
            if (structIt != structTypes.end()) {
                llvm::StructType* structType = structIt->second;
                
                // Cast self pointer to the correct struct type
                llvm::Value* typedSelfPtr = builder.CreateBitCast(selfPtr, llvm::PointerType::get(structType, 0), "typed_self");
                
                // Get pointer to the member
                llvm::Value* memberPtr = builder.CreateStructGEP(structType, typedSelfPtr, attrIndex, "member_ptr");
                
                // Load the member value
                llvm::Type* memberType = structType->getElementType(attrIndex);
                lastValue = builder.CreateLoad(memberType, memberPtr, "member_value");
                
                std::cerr << "DEBUG: Successfully accessed member '" << node.attributeName << "' at index " << attrIndex << std::endl;
            } else {
                std::cerr << "Error: Could not find struct type for " << selfTypeName << std::endl;
                lastValue = nullptr;
            }
        } else {
            std::cerr << "Error: Member '" << node.attributeName << "' not found in type '" << selfTypeName << "'" << std::endl;
            lastValue = nullptr;
        }
    } else {
        std::cerr << "Error: Type definition not found for self (selfTypeName='" << selfTypeName << "')" << std::endl;
        lastValue = nullptr;
    }
}

void LLVMCodegenVisitor::visit(MethodCallNode& node) {
    // Evaluate the object expression
    node.object->accept(*this);
    llvm::Value* objPtr = lastValue;
    
    if (!objPtr) {
        std::cerr << "Error: Invalid object in method call" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    // Determine the type of the object
    std::string objectTypeName = "";
    
    // Try to get the type from the object expression
    if (auto varNode = dynamic_cast<VariableNode*>(node.object)) {
        // Look up the variable type
        auto typeIt = variableTypes.find(varNode->identifier);
        if (typeIt != variableTypes.end()) {
            objectTypeName = typeIt->second;
        }
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.object)) {
        // Direct type instantiation
        objectTypeName = typeInstNode->typeName;
    }
    
    // If we couldn't determine the type, try to infer it from the pointer type
    if (objectTypeName.empty()) {
        // Look through struct types to find a match
        for (const auto& structPair : structTypes) {
            llvm::Type* expectedPtrType = llvm::PointerType::get(structPair.second, 0);
            if (objPtr->getType() == expectedPtrType) {
                objectTypeName = structPair.first;
                break;
            }
        }
    }
    
    // Evaluate arguments
    std::vector<llvm::Value*> args;
    args.push_back(objPtr); // Add 'self' as first argument
    
    for (auto arg : node.arguments) {
        arg->accept(*this);
        if (lastValue) {
            args.push_back(lastValue);
        }
    }
    
    // Construct the method function name using the pattern "TypeName_methodName"
    std::string methodFuncName = node.methodName; // Default fallback
    if (!objectTypeName.empty()) {
        methodFuncName = objectTypeName + "_" + node.methodName;
    }
    
    std::cerr << "DEBUG: Looking for method '" << methodFuncName << "' (object type: '" << objectTypeName << "')" << std::endl;
    
    llvm::Function* methodFunc = module.getFunction(methodFuncName);
    
    if (methodFunc) {
        // Call the method
        lastValue = builder.CreateCall(methodFunc, args, "method_result");
        std::cerr << "DEBUG: Successfully called method '" << methodFuncName << "'" << std::endl;
    } else {
        std::cerr << "Error: Method '" << methodFuncName << "' not found" << std::endl;
        lastValue = nullptr;
    }
}