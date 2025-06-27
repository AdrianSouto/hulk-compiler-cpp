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
#include "Expressions/FuncCallNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Globals.hpp"
#include "RuntimeTypeInfo.hpp"

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
        size_t currentIndex = 1; 
        
        
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
                        
                        convertedVal = builder.CreateFPToSI(val, memberType, "double_to_int");
                        std::cerr << "DEBUG: Applied double to int conversion" << std::endl;
                    } else if (val->getType()->isIntegerTy(32) && memberType->isDoubleTy()) {
                        
                        convertedVal = builder.CreateSIToFP(val, memberType, "int_to_double");
                        std::cerr << "DEBUG: Applied int to double conversion" << std::endl;
                    } else if (val->getType()->isPointerTy() && memberType->isPointerTy()) {
                        
                        convertedVal = builder.CreateBitCast(val, memberType, "ptr_cast");
                        std::cerr << "DEBUG: Applied pointer cast conversion" << std::endl;
                    } else {
                        
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
    
    node.object->accept(*this);
    llvm::Value* objPtr = lastValue;
    
    if (!objPtr) {
        std::cerr << "Error: Invalid object in member access" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
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
        size_t currentIndex = 1; 

        
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
    
    
    std::string selfTypeName = currentTypeName.empty() ? "Object" : currentTypeName;
    
    
    auto typeIt = types.find(selfTypeName);
    if (typeIt != types.end()) {
        TypeDefNode* typeDef = typeIt->second;
        
        int attrIndex = -1;
        size_t currentIndex = 1; 
        
        
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
            
            auto structIt = structTypes.find(selfTypeName);
            if (structIt != structTypes.end()) {
                llvm::StructType* structType = structIt->second;
                
                
                llvm::Value* typedSelfPtr = builder.CreateBitCast(selfPtr, llvm::PointerType::get(structType, 0), "typed_self");
                
                
                llvm::Value* memberPtr = builder.CreateStructGEP(structType, typedSelfPtr, attrIndex, "member_ptr");
                
                
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
    
    node.object->accept(*this);
    llvm::Value* objPtr = lastValue;
    
    if (!objPtr) {
        std::cerr << "Error: Invalid object in method call" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    
    SimpleRuntimeTypeSystem& rts = SimpleRuntimeTypeSystem::getInstance();
    rts.initialize(module, ctx);
    
    
    std::string staticTypeName = "";
    
    
    if (auto varNode = dynamic_cast<VariableNode*>(node.object)) {
        
        if (varNode->identifier == "self" && !currentTypeName.empty()) {
            staticTypeName = currentTypeName;
            std::cerr << "DEBUG: 'self' has current type '" << staticTypeName << "'" << std::endl;
        } else {
            
            auto typeIt = variableTypes.find(varNode->identifier);
            if (typeIt != variableTypes.end()) {
                staticTypeName = typeIt->second;
            }
            std::cerr << "DEBUG: Variable '" << varNode->identifier << "' has static type '" << staticTypeName << "'" << std::endl;
        }
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.object)) {
        
        staticTypeName = typeInstNode->typeName;
        std::cerr << "DEBUG: Direct type instantiation of '" << staticTypeName << "'" << std::endl;
    } else if (auto funcCallNode = dynamic_cast<FuncCallNode*>(node.object)) {
        
        auto typeIt = variableTypes.find("_last_call_result");
        if (typeIt != variableTypes.end()) {
            staticTypeName = typeIt->second;
            std::cerr << "DEBUG: Function call returns type '" << staticTypeName << "'" << std::endl;
        }
    }
    
    
    std::vector<llvm::Value*> args;
    args.push_back(objPtr); 
    
    for (auto arg : node.arguments) {
        arg->accept(*this);
        if (lastValue) {
            args.push_back(lastValue);
        }
    }
    
    
    
    bool isCustomType = staticTypeName != "Number" && staticTypeName != "String" && 
                       staticTypeName != "Boolean" && !staticTypeName.empty();
    
    if (isCustomType && objPtr->getType()->isPointerTy()) {
        
        std::string dispatchFuncName = "dispatch_" + node.methodName + "_" + staticTypeName;
        llvm::Function* dispatchFunc = module.getFunction(dispatchFuncName);
        
        if (!dispatchFunc) {
            
            std::vector<llvm::Type*> paramTypes;
            for (auto& arg : args) {
                paramTypes.push_back(arg->getType());
            }
            
            
            llvm::Type* returnType = llvm::Type::getDoubleTy(ctx); 
            
            
            bool foundMethod = false;
            std::function<void(const std::string&)> searchForMethod = [&](const std::string& typeName) {
                if (foundMethod || typeName.empty()) return;
                
                auto typeIt = types.find(typeName);
                if (typeIt != types.end()) {
                    TypeDefNode* typeDef = typeIt->second;
                    
                    
                    for (auto method : typeDef->methods) {
                        if (auto defFunc = dynamic_cast<DefFuncNode*>(method)) {
                            if (defFunc->identifier == node.methodName) {
                                foundMethod = true;
                                if (defFunc->returnType) {
                                    std::string returnTypeName = defFunc->returnType->toString();
                                    if (returnTypeName == "Number") {
                                        returnType = llvm::Type::getDoubleTy(ctx);
                                    } else if (returnTypeName == "String") {
                                        returnType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
                                    } else if (returnTypeName == "Boolean") {
                                        returnType = llvm::Type::getInt1Ty(ctx);
                                    } else {
                                        
                                        auto structIt = structTypes.find(returnTypeName);
                                        if (structIt != structTypes.end()) {
                                            returnType = llvm::PointerType::get(structIt->second, 0);
                                        } else {
                                            returnType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
                                        }
                                    }
                                } else {
                                    
                                    returnType = llvm::Type::getDoubleTy(ctx);
                                }
                                return;
                            }
                        }
                    }
                    
                    
                    searchForMethod(typeDef->parentTypeName);
                }
            };
            
            searchForMethod(staticTypeName);
            
            llvm::FunctionType* dispatchFuncType = llvm::FunctionType::get(
                returnType, paramTypes, false);
            
            dispatchFunc = llvm::Function::Create(
                dispatchFuncType, llvm::Function::InternalLinkage, 
                dispatchFuncName, module);
            
            
            llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(ctx, "entry", dispatchFunc);
            llvm::IRBuilder<> dispatchBuilder(entryBB);
            
            auto argIt = dispatchFunc->arg_begin();
            llvm::Value* selfArg = &*argIt++;
            
            
            llvm::Value* objAsObjectPtr = dispatchBuilder.CreateBitCast(
                selfArg, llvm::PointerType::get(rts.getObjectStructType(ctx), 0), "obj_as_object");
            
            llvm::Value* typeInfoPtrPtr = dispatchBuilder.CreateStructGEP(
                rts.getObjectStructType(ctx), objAsObjectPtr, 0, "typeinfo_ptr_ptr");
            
            llvm::Value* typeInfoPtr = dispatchBuilder.CreateLoad(
                llvm::PointerType::get(rts.getTypeInfoStructType(ctx), 0), 
                typeInfoPtrPtr, "typeinfo_ptr");
            
            
            llvm::Value* typeNamePtrPtr = dispatchBuilder.CreateStructGEP(
                rts.getTypeInfoStructType(ctx), typeInfoPtr, 0, "typename_ptr_ptr");
            
            llvm::Value* typeNamePtr = dispatchBuilder.CreateLoad(
                llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0), 
                typeNamePtrPtr, "typename_ptr");
            
            
            std::vector<std::string> possibleTypes;
            std::map<std::string, llvm::BasicBlock*> typeBlocks;
            llvm::BasicBlock* defaultBB = llvm::BasicBlock::Create(ctx, "default", dispatchFunc);
            
            
            std::function<void(const std::string&)> collectTypes = [&](const std::string& typeName) {
                if (typeName.empty() || typeBlocks.count(typeName) > 0) return;
                
                possibleTypes.push_back(typeName);
                typeBlocks[typeName] = llvm::BasicBlock::Create(ctx, "type_" + typeName, dispatchFunc);
                
                
                for (const auto& typePair : types) {
                    if (typePair.second->parentTypeName == typeName) {
                        collectTypes(typePair.first);
                    }
                }
            };
            
            collectTypes(staticTypeName);
            
            
            llvm::BasicBlock* currentBB = entryBB;
            for (size_t i = 0; i < possibleTypes.size(); ++i) {
                const std::string& typeName = possibleTypes[i];
                llvm::BasicBlock* nextCheckBB = (i + 1 < possibleTypes.size()) ? 
                    llvm::BasicBlock::Create(ctx, "check_" + possibleTypes[i+1], dispatchFunc) : defaultBB;
                
                dispatchBuilder.SetInsertPoint(currentBB);
                
                
                llvm::Value* expectedTypeName = dispatchBuilder.CreateGlobalStringPtr(typeName, "expected_type_" + typeName);
                
                
                llvm::Function* strcmpFunc = module.getFunction("strcmp");
                if (!strcmpFunc) {
                    llvm::FunctionType* strcmpType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(ctx),
                        {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),
                         llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
                        false
                    );
                    strcmpFunc = llvm::Function::Create(strcmpType, llvm::Function::ExternalLinkage, "strcmp", module);
                }
                
                llvm::Value* cmpResult = dispatchBuilder.CreateCall(strcmpFunc, {typeNamePtr, expectedTypeName}, "strcmp_result");
                llvm::Value* isEqual = dispatchBuilder.CreateICmpEQ(cmpResult, 
                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0), "is_type_" + typeName);
                
                dispatchBuilder.CreateCondBr(isEqual, typeBlocks[typeName], nextCheckBB);
                
                
                dispatchBuilder.SetInsertPoint(typeBlocks[typeName]);
                
                
                llvm::Function* methodFunc = nullptr;
                std::string methodOwnerType = typeName;
                
                
                while (!methodOwnerType.empty() && !methodFunc) {
                    std::string methodName = methodOwnerType + "_" + node.methodName;
                    methodFunc = module.getFunction(methodName);
                    
                    if (!methodFunc) {
                        
                        auto typeIt = types.find(methodOwnerType);
                        if (typeIt != types.end() && !typeIt->second->parentTypeName.empty()) {
                            methodOwnerType = typeIt->second->parentTypeName;
                        } else {
                            break;
                        }
                    }
                }
                
                if (methodFunc) {
                    
                    std::vector<llvm::Value*> methodArgs;
                    argIt = dispatchFunc->arg_begin();
                    for (auto& arg : args) {
                        methodArgs.push_back(&*argIt++);
                    }
                    
                    llvm::Value* result = dispatchBuilder.CreateCall(methodFunc, methodArgs, "method_result");
                    dispatchBuilder.CreateRet(result);
                } else {
                    
                    dispatchBuilder.CreateBr(defaultBB);
                }
                
                currentBB = nextCheckBB;
            }
            
            
            dispatchBuilder.SetInsertPoint(defaultBB);
            
            
            llvm::Value* defaultReturnValue = nullptr;
            if (returnType->isDoubleTy()) {
                defaultReturnValue = llvm::ConstantFP::get(returnType, 0.0);
            } else if (returnType->isIntegerTy()) {
                defaultReturnValue = llvm::ConstantInt::get(returnType, 0);
            } else if (returnType->isPointerTy()) {
                defaultReturnValue = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(returnType));
            } else {
                
                defaultReturnValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), 0.0);
            }
            
            dispatchBuilder.CreateRet(defaultReturnValue);
        }
        
        
        lastValue = builder.CreateCall(dispatchFunc, args, "dispatched_result");
        std::cerr << "DEBUG: Using dynamic dispatch for method '" << node.methodName << "'" << std::endl;
        
    } else {
        
        
        std::vector<std::string> typesToTry;
        if (!staticTypeName.empty()) {
            typesToTry.push_back(staticTypeName);

            
            std::string currentParentName = staticTypeName;
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
        }

        std::cerr << "DEBUG: Static dispatch - looking for method '" << node.methodName << "' in types: ";
        for (const auto& type : typesToTry) {
            std::cerr << type << " ";
        }
        std::cerr << std::endl;
        
        
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
            
            lastValue = builder.CreateCall(methodFunc, args, "method_result");
            std::cerr << "DEBUG: Successfully called method" << std::endl;
        } else {
            std::cerr << "Error: Method '" << node.methodName << "' not found for object type" << std::endl;
            lastValue = nullptr;
        }
    }
}
