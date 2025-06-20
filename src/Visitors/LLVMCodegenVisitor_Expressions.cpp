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
        size_t currentIndex = 1; // Start from 1 to skip TypeInfo* field
        
        // Build inheritance chain
        std::vector<TypeDefNode*> inheritanceChain;
        std::function<void(const std::string&)> buildChain = [&](const std::string& typeName) {
            if (typeName.empty()) return;

            auto typeIt = types.find(typeName);
            if (typeIt != types.end()) {
                TypeDefNode* currentTypeDef = typeIt->second;

                if (!currentTypeDef->parentTypeName.empty()) {
                    buildChain(currentTypeDef->parentTypeName);
                }

                inheritanceChain.push_back(currentTypeDef);
            }
        };

        buildChain(selfTypeName);

        // Search for the attribute in the inheritance chain
        for (TypeDefNode* chainTypeDef : inheritanceChain) {
            for (size_t i = 0; i < chainTypeDef->attributes.size(); ++i) {
                if (chainTypeDef->attributes[i].name == node.member) {
                    attrIndex = currentIndex;
                    break;
                }
                currentIndex++;
            }
            if (attrIndex >= 0) break;
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
                
                std::cerr << "DEBUG: Assigning to member '" << node.member << "' at index " << attrIndex << std::endl;
                std::cerr << "DEBUG: Value type: " << (val->getType()->isPointerTy() ? "pointer" : 
                    (val->getType()->isDoubleTy() ? "double" : 
                    (val->getType()->isIntegerTy() ? "integer" : "other"))) << std::endl;
                std::cerr << "DEBUG: Member type: " << (memberType->isPointerTy() ? "pointer" : 
                    (memberType->isDoubleTy() ? "double" : 
                    (memberType->isIntegerTy() ? "integer" : "other"))) << std::endl;
                
                if (val->getType() != memberType) {
                    std::cerr << "DEBUG: Type conversion needed" << std::endl;
                    if (val->getType()->isDoubleTy() && memberType->isIntegerTy(32)) {
                        // Convert double to i32
                        convertedVal = builder.CreateFPToSI(val, memberType, "double_to_int");
                        std::cerr << "DEBUG: Applied double to int conversion" << std::endl;
                    } else if (val->getType()->isIntegerTy(32) && memberType->isDoubleTy()) {
                        // Convert i32 to double
                        convertedVal = builder.CreateSIToFP(val, memberType, "int_to_double");
                        std::cerr << "DEBUG: Applied int to double conversion" << std::endl;
                    } else if (val->getType()->isPointerTy() && memberType->isPointerTy()) {
                        // Convert between pointer types (e.g., different string pointer types)
                        convertedVal = builder.CreateBitCast(val, memberType, "ptr_cast");
                        std::cerr << "DEBUG: Applied pointer cast conversion" << std::endl;
                    } else {
                        // For other type mismatches, try a bitcast
                        try {
                            convertedVal = builder.CreateBitCast(val, memberType, "type_cast");
                            std::cerr << "DEBUG: Applied general bitcast conversion" << std::endl;
                        } catch (...) {
                            std::cerr << "DEBUG: Bitcast failed, using original value" << std::endl;
                            convertedVal = val;
                        }
                    }
                } else {
                    std::cerr << "DEBUG: No type conversion needed" << std::endl;
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
    
    // Determine the type of the object
    std::string objTypeName = "Object";

    if (auto varNode = dynamic_cast<VariableNode*>(node.object)) {
        auto typeIt = variableTypes.find(varNode->identifier);
        if (typeIt != variableTypes.end()) {
            objTypeName = typeIt->second;
        }
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.object)) {
        objTypeName = typeInstNode->typeName;
    }

    auto typeIt = types.find(objTypeName);
    if (typeIt != types.end()) {
        TypeDefNode* typeDef = typeIt->second;

        int attrIndex = -1;
        size_t currentIndex = 1; // Start from 1 to skip TypeInfo* field

        // Build inheritance chain
        std::vector<TypeDefNode*> inheritanceChain;
        std::function<void(const std::string&)> buildChain = [&](const std::string& typeName) {
            if (typeName.empty()) return;

            auto typeIt = types.find(typeName);
            if (typeIt != types.end()) {
                TypeDefNode* currentTypeDef = typeIt->second;

                if (!currentTypeDef->parentTypeName.empty()) {
                    buildChain(currentTypeDef->parentTypeName);
                }

                inheritanceChain.push_back(currentTypeDef);
            }
        };

        buildChain(objTypeName);

        // Search for the attribute in the inheritance chain
        for (TypeDefNode* chainTypeDef : inheritanceChain) {
            for (size_t i = 0; i < chainTypeDef->attributes.size(); ++i) {
                if (chainTypeDef->attributes[i].name == node.memberName) {
                    attrIndex = currentIndex;
                    break;
                }
                currentIndex++;
            }
            if (attrIndex >= 0) break;
        }

        if (attrIndex >= 0) {
            auto structIt = structTypes.find(objTypeName);
            if (structIt != structTypes.end()) {
                llvm::StructType* structType = structIt->second;

                llvm::Value* memberPtr = builder.CreateStructGEP(structType, objPtr, attrIndex, "member_ptr");
                llvm::Type* memberType = structType->getElementType(attrIndex);
                lastValue = builder.CreateLoad(memberType, memberPtr, "member_value");

                std::cerr << "DEBUG: Successfully loaded member '" << node.memberName << "' from type '" << objTypeName << "'" << std::endl;
            } else {
                std::cerr << "Error: Could not find struct type for " << objTypeName << std::endl;
                lastValue = nullptr;
            }
        } else {
            std::cerr << "Error: Member '" << node.memberName << "' not found in type '" << objTypeName << "'" << std::endl;
            lastValue = nullptr;
        }
    } else {
        std::cerr << "Error: Type definition not found for object type '" << objTypeName << "'" << std::endl;
        lastValue = nullptr;
    }
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
        
        // Build inheritance chain
        std::vector<TypeDefNode*> inheritanceChain;
        std::function<void(const std::string&)> buildChain = [&](const std::string& typeName) {
            if (typeName.empty()) return;

            auto typeIt = types.find(typeName);
            if (typeIt != types.end()) {
                TypeDefNode* currentTypeDef = typeIt->second;

                if (!currentTypeDef->parentTypeName.empty()) {
                    buildChain(currentTypeDef->parentTypeName);
                }

                inheritanceChain.push_back(currentTypeDef);
            }
        };

        buildChain(selfTypeName);

        // Search for the attribute in the inheritance chain
        for (TypeDefNode* chainTypeDef : inheritanceChain) {
            for (size_t i = 0; i < chainTypeDef->attributes.size(); ++i) {
                if (chainTypeDef->attributes[i].name == node.attributeName) {
                    attrIndex = currentIndex;
                    break;
                }
                currentIndex++;
            }
            if (attrIndex >= 0) break;
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
        std::cerr << "DEBUG: Variable '" << varNode->identifier << "' has type '" << objectTypeName << "'" << std::endl;
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.object)) {
        // Direct type instantiation
        objectTypeName = typeInstNode->typeName;
        std::cerr << "DEBUG: Direct type instantiation of '" << objectTypeName << "'" << std::endl;
    }
    
    // Build list of types to try (including inheritance)
    std::vector<std::string> typesToTry;
    if (!objectTypeName.empty()) {
        typesToTry.push_back(objectTypeName);

        // Add parent types
        std::string currentParentName = objectTypeName;
        auto typeIt = types.find(currentParentName);
        if (typeIt != types.end()) {
            currentParentName = typeIt->second->parentTypeName;
            while (!currentParentName.empty()) {
                typesToTry.push_back(currentParentName);
                auto parentTypeIt = types.find(currentParentName);
                if (parentTypeIt != types.end()) {
                    currentParentName = parentTypeIt->second->parentTypeName;
                } else {
                    break;
                }
            }
        }
    } else {
        // If we can't determine the type, try all types
        for (const auto& typePair : types) {
            typesToTry.push_back(typePair.first);
        }
    }

    std::cerr << "DEBUG: Looking for method '" << node.methodName << "' in types: ";
    for (const auto& type : typesToTry) {
        std::cerr << type << " ";
    }
    std::cerr << std::endl;
    
    // Evaluate arguments
    std::vector<llvm::Value*> args;
    args.push_back(objPtr); // Add 'self' as first argument
    
    for (auto arg : node.arguments) {
        arg->accept(*this);
        if (lastValue) {
            args.push_back(lastValue);
        }
    }
    
    // Search for the method function
    llvm::Function* methodFunc = nullptr;
    for (const std::string& typeName : typesToTry) {
        std::string methodName = typeName + "_" + node.methodName;
        std::cerr << "DEBUG: Trying method name: " << methodName << std::endl;
        methodFunc = module.getFunction(methodName);
        if (methodFunc) {
            std::cerr << "DEBUG: Found method: " << methodName << std::endl;
            break;
        }
    }
    
    if (methodFunc) {
        // Call the method
        lastValue = builder.CreateCall(methodFunc, args, "method_result");
        std::cerr << "DEBUG: Successfully called method" << std::endl;
    } else {
        std::cerr << "Error: Method '" << node.methodName << "' not found for object type" << std::endl;
        lastValue = nullptr;
    }
}