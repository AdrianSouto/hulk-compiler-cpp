#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/SelfMemberAccessNode.hpp"
#include "Expressions/MemberAccessNode.hpp"
#include "Expressions/MethodCallNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Expressions/IsNode.hpp"
#include "Expressions/AsNode.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <iostream>


std::map<std::string, llvm::StructType*> structTypes;


static llvm::Type* getLLVMTypeFromName(const std::string& typeName, llvm::LLVMContext& ctx) {
    if (typeName == "Number") {
        return llvm::Type::getDoubleTy(ctx);  // Changed from int32 to double for consistency
    } else if (typeName == "String") {
        return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    } else if (typeName == "Boolean") {
        return llvm::Type::getInt1Ty(ctx);
    } else if (typeName == "Object") {
        return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    }

    return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
}


static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(TypeDefNode& node) {
    std::vector<llvm::Type*> memberTypes;
    std::vector<std::string> memberNames;
    

    std::function<void(const std::string&)> addInheritedAttributes = [&](const std::string& typeName) {
        if (typeName.empty()) return;
        
        auto typeIt = types.find(typeName);
        if (typeIt != types.end()) {
            TypeDefNode* typeDef = typeIt->second;
            

            if (!typeDef->parentTypeName.empty()) {
                addInheritedAttributes(typeDef->parentTypeName);
            }
            

            for (const auto& attr : typeDef->attributes) {
                std::string attrTypeName = "Number";
                if (attr.type) {
                    attrTypeName = attr.type->toString();
                }
                llvm::Type* attrType = getLLVMTypeFromName(attrTypeName, ctx);
                memberTypes.push_back(attrType);
                memberNames.push_back(attr.name);
            }
        }
    };
    

    if (!node.parentTypeName.empty()) {
        addInheritedAttributes(node.parentTypeName);
    }
    

    for (const auto& attr : node.attributes) {
        std::string typeName = "Number";
        if (attr.type) {
            typeName = attr.type->toString();
        }
        llvm::Type* attrType = getLLVMTypeFromName(typeName, ctx);
        memberTypes.push_back(attrType);
        memberNames.push_back(attr.name);
    }
    

    if (memberTypes.empty()) {
        memberTypes.push_back(llvm::Type::getInt8Ty(ctx));
    }
    

    llvm::StructType* structType = llvm::StructType::create(ctx, memberTypes, node.typeName);
    structTypes[node.typeName] = structType;
    

    std::cerr << "DEBUG: Type " << node.typeName << " has " << node.methods.size() << " methods" << std::endl;
    for (auto method : node.methods) {
        if (auto defFunc = dynamic_cast<DefFuncNode*>(method)) {
            std::string originalName = defFunc->identifier;
            defFunc->identifier = node.typeName + "_" + originalName;
            std::cerr << "DEBUG: Processing method " << originalName << " as " << defFunc->identifier << std::endl;
            

            bool hasSelfParam = false;
            for (const auto& param : defFunc->parameters) {
                if (param.name == "self") {
                    hasSelfParam = true;
                    break;
                }
            }
            
            if (!hasSelfParam) {
                Parameter selfParam("self", nullptr);
                defFunc->parameters.insert(defFunc->parameters.begin(), selfParam);
            }
            

            std::string previousTypeName = currentTypeName;
            currentTypeName = node.typeName;
            

            defFunc->accept(*this);
            

            currentTypeName = previousTypeName;
            defFunc->identifier = originalName;
        }
    }
    

    std::string constructorName = "new_" + node.typeName;
    std::vector<llvm::Type*> constructorParamTypes;
    

    // Create effective type arguments for constructor parameters
    // If the type has explicit type arguments, use those
    // If not, create implicit type arguments for inherited attributes
    std::vector<Parameter> effectiveTypeArguments = node.typeArguments;
    
    if (effectiveTypeArguments.empty() && !node.parentTypeName.empty()) {
        // Create implicit type arguments for inherited attributes
        std::function<void(const std::string&)> collectInheritedAttributes = [&](const std::string& typeName) {
            if (typeName.empty()) return;
            
            auto typeIt = types.find(typeName);
            if (typeIt != types.end()) {
                TypeDefNode* typeDef = typeIt->second;
                
                // First collect from parent
                if (!typeDef->parentTypeName.empty()) {
                    collectInheritedAttributes(typeDef->parentTypeName);
                }
                
                // Then add this type's attributes as implicit type arguments
                for (const auto& attr : typeDef->attributes) {
                    Parameter implicitParam(attr.name, attr.type);
                    effectiveTypeArguments.push_back(implicitParam);
                }
            }
        };
        
        collectInheritedAttributes(node.parentTypeName);
    }
    
    // Build constructor parameter types
    for (const auto& param : effectiveTypeArguments) {
        std::string typeName = "Number";
        if (param.type) {
            typeName = param.type->toString();
        }
        llvm::Type* paramType = getLLVMTypeFromName(typeName, ctx);
        constructorParamTypes.push_back(paramType);
    }
    

    llvm::FunctionType* constructorType = llvm::FunctionType::get(
        llvm::PointerType::get(structType, 0),
        constructorParamTypes,
        false
    );
    
    llvm::Function* constructorFunc = llvm::Function::Create(
        constructorType, 
        llvm::Function::ExternalLinkage, 
        constructorName, 
        module
    );
    

    llvm::BasicBlock* constructorBB = llvm::BasicBlock::Create(ctx, "entry", constructorFunc);
    llvm::IRBuilder<> constructorBuilder(constructorBB);
    

    llvm::Value* structSize = llvm::ConstantExpr::getSizeOf(structType);
    llvm::Function* mallocFunc = module.getFunction("malloc");
    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),
            {llvm::Type::getInt64Ty(ctx)},
            false
        );
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module);
    }
    
    llvm::Value* rawPtr = constructorBuilder.CreateCall(mallocFunc, {structSize});
    llvm::Value* typedPtr = constructorBuilder.CreateBitCast(rawPtr, llvm::PointerType::get(structType, 0));
    

    auto argIt = constructorFunc->arg_begin();
    size_t memberIndex = 0;
    

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
    
    buildChain(node.typeName);
    
    // Create a map to store constructor parameters by name
    std::map<std::string, llvm::Value*> constructorParams;
    auto paramIt = constructorFunc->arg_begin();
    
    // Collect constructor parameters using effective type arguments
    for (const auto& param : effectiveTypeArguments) {
        if (paramIt != constructorFunc->arg_end()) {
            constructorParams[param.name] = &*paramIt;
            ++paramIt;
        }
    }
    
    // Initialize members
    for (size_t chainIndex = 0; chainIndex < inheritanceChain.size(); ++chainIndex) {
        TypeDefNode* chainTypeDef = inheritanceChain[chainIndex];
        
        // Check if this is a parent type and if the current node has parent initialization expressions
        bool useParentInitExpressions = false;
        std::vector<llvm::Value*> parentInitValues;
        
        if (chainTypeDef->typeName == node.parentTypeName && !node.parentArgs.empty()) {
            useParentInitExpressions = true;
            
            // Evaluate parent initialization expressions
            for (auto parentArg : node.parentArgs) {
                // Save current insertion point
                auto savedInsertPoint = builder.GetInsertBlock();
                auto savedInsertPointIt = builder.GetInsertPoint();
                builder.SetInsertPoint(constructorBB);
                
                // Create a temporary context with constructor parameters
                std::map<std::string, llvm::AllocaInst*> tempVars;
                for (const auto& param : constructorParams) {
                    llvm::AllocaInst* alloca = createEntryBlockAlloca(constructorFunc, param.second->getType(), param.first);
                    constructorBuilder.CreateStore(param.second, alloca);
                    tempVars[param.first] = alloca;
                }
                localVarsStack.push_back(tempVars);
                
                // Evaluate the expression
                parentArg->accept(*this);
                llvm::Value* evalValue = lastValue;
                
                // Restore context
                localVarsStack.pop_back();
                builder.SetInsertPoint(savedInsertPoint, savedInsertPointIt);
                
                if (evalValue) {
                    parentInitValues.push_back(evalValue);
                } else {
                    // Default value if evaluation fails
                    parentInitValues.push_back(llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), 0.0));
                }
            }
        }
        
        for (size_t i = 0; i < chainTypeDef->attributes.size(); ++i) {
            llvm::Value* memberPtr = constructorBuilder.CreateStructGEP(structType, typedPtr, memberIndex);
            llvm::Value* initValue = nullptr;
            
            // Check if we should use parent initialization expressions
            if (useParentInitExpressions && i < parentInitValues.size()) {
                initValue = parentInitValues[i];
            }
            // Check if this attribute corresponds to a type argument
            else if (i < chainTypeDef->typeArguments.size() && chainTypeDef->typeArguments[i].name == chainTypeDef->attributes[i].name) {
                // Find the parameter value by name
                auto paramIt = constructorParams.find(chainTypeDef->typeArguments[i].name);
                if (paramIt != constructorParams.end()) {
                    initValue = paramIt->second;
                }
            }
            // For inherited types, check if attribute matches constructor parameter
            else if (chainTypeDef != &node && node.parentArgs.empty() && !node.parentTypeName.empty()) {
                // Check if this inherited attribute name matches any of the effective type arguments
                for (const auto& param : effectiveTypeArguments) {
                    if (param.name == chainTypeDef->attributes[i].name) {
                        auto paramIt = constructorParams.find(param.name);
                        if (paramIt != constructorParams.end()) {
                            initValue = paramIt->second;
                            break;
                        }
                    }
                }
            }
            
            // If no init value yet, try the init expression
            if (!initValue && chainTypeDef->attributes[i].hasInitExpression()) {
                    // For now, just use constant values for initial expressions
                    // TODO: Properly evaluate init expressions in constructor context
                    std::string attrTypeName = "Number";
                    if (chainTypeDef->attributes[i].type) {
                        attrTypeName = chainTypeDef->attributes[i].type->toString();
                    }
                    
                    // Check if it's a simple number literal
                    if (auto numNode = dynamic_cast<NumberNode*>(chainTypeDef->attributes[i].initExpression)) {
                        if (attrTypeName == "Number") {
                            initValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), numNode->value);
                        } else {
                            initValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), numNode->value);
                        }
                    } else if (auto strNode = dynamic_cast<StringLiteralNode*>(chainTypeDef->attributes[i].initExpression)) {
                        // Handle string literal initialization
                        initValue = constructorBuilder.CreateGlobalStringPtr(strNode->value);
                    } else if (auto boolNode = dynamic_cast<BooleanNode*>(chainTypeDef->attributes[i].initExpression)) {
                        // Handle boolean literal initialization
                        initValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), boolNode->value ? 1 : 0);
                    } else {
                        // Default value for complex expressions
                        if (attrTypeName == "Number") {
                            initValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), 0.0);
                        } else if (attrTypeName == "Boolean") {
                            initValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), 0);
                        } else {
                            initValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
                        }
                    }
            }
            
            // If still no init value, use default
            if (!initValue) {
                std::string attrTypeName = "Number";
                    if (chainTypeDef->attributes[i].type) {
                        attrTypeName = chainTypeDef->attributes[i].type->toString();
                    }
                    
                    if (attrTypeName == "Number") {
                        initValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), 0.0);
                    } else if (attrTypeName == "Boolean") {
                        initValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), 0);
                    } else if (attrTypeName == "String") {
                        initValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
                    } else {
                        initValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
                    }
                }
            
            constructorBuilder.CreateStore(initValue, memberPtr);
            memberIndex++;
        }
    }
    
    constructorBuilder.CreateRet(typedPtr);
    lastValue = nullptr;
}

void LLVMCodegenVisitor::visit(TypeInstantiationNode& node) {

    auto typeIt = types.find(node.typeName);
    if (typeIt == types.end()) {
        std::cerr << "Error: Unknown type '" << node.typeName << "'" << std::endl;
        lastValue = nullptr;
        return;
    }
    

    std::string constructorName = "new_" + node.typeName;
    llvm::Function* constructorFunc = module.getFunction(constructorName);
    
    if (!constructorFunc) {
        std::cerr << "Error: Constructor not found for type '" << node.typeName << "'" << std::endl;
        lastValue = nullptr;
        return;
    }
    

    std::vector<llvm::Value*> args;
    auto paramIt = constructorFunc->arg_begin();
    for (auto arg : node.arguments) {
        arg->accept(*this);
        if (lastValue && paramIt != constructorFunc->arg_end()) {
            llvm::Value* argVal = lastValue;
            llvm::Type* expectedType = paramIt->getType();
            

            if (argVal->getType() != expectedType) {
                if (argVal->getType()->isIntegerTy() && expectedType->isIntegerTy()) {

                    if (argVal->getType()->getIntegerBitWidth() < expectedType->getIntegerBitWidth()) {
                        argVal = builder.CreateZExt(argVal, expectedType, "arg_int_extend");
                    } else if (argVal->getType()->getIntegerBitWidth() > expectedType->getIntegerBitWidth()) {
                        argVal = builder.CreateTrunc(argVal, expectedType, "arg_int_trunc");
                    }
                }
            }
            
            args.push_back(argVal);
            ++paramIt;
        } else if (lastValue) {
            args.push_back(lastValue);
        }
    }
    

    lastValue = builder.CreateCall(constructorFunc, args, "new_instance");
    

    variableTypes["temp_instance"] = node.typeName;
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
        std::cerr << "Error: 'self' not found in current context" << std::endl;
        lastValue = nullptr;
        return;
    }
    
    std::string selfTypeName = currentTypeName.empty() ? "Object" : currentTypeName;
    
    std::cerr << "DEBUG: SelfMemberAccessNode - currentTypeName='" << currentTypeName << "', selfTypeName='" << selfTypeName << "'" << std::endl;
    
    auto typeIt = types.find(selfTypeName);
    if (typeIt != types.end()) {
        TypeDefNode* typeDef = typeIt->second;
        
        int attrIndex = -1;
        size_t currentIndex = 0;
        

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
            std::cerr << "DEBUG: Found attribute '" << node.attributeName << "' at index " << attrIndex << std::endl;
            
            auto structIt = structTypes.find(selfTypeName);
            if (structIt != structTypes.end()) {
                llvm::StructType* structType = structIt->second;
                
                llvm::Value* memberPtr = builder.CreateStructGEP(structType, selfPtr, attrIndex, "attr_ptr");
                llvm::Type* memberType = structType->getElementType(attrIndex);
                lastValue = builder.CreateLoad(memberType, memberPtr, "attr_value");
                
                std::cerr << "DEBUG: Successfully loaded attribute '" << node.attributeName << "'" << std::endl;
            } else {
                std::cerr << "Error: Could not find struct type for " << selfTypeName << std::endl;
                lastValue = nullptr;
            }
        } else {
            std::cerr << "Error: Attribute '" << node.attributeName << "' not found in type '" << selfTypeName << "'" << std::endl;
            lastValue = nullptr;
        }
    } else {
        std::cerr << "Error: Type definition not found for self (selfTypeName='" << selfTypeName << "')" << std::endl;
        std::cerr << "Available types: ";
        for (const auto& typePair : types) {
            std::cerr << "'" << typePair.first << "' ";
        }
        std::cerr << std::endl;
        lastValue = nullptr;
    }
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
    
    auto typeIt = types.find(objTypeName);
    if (typeIt != types.end()) {
        TypeDefNode* typeDef = typeIt->second;
        

        int memberIndex = -1;
        for (size_t i = 0; i < typeDef->attributes.size(); ++i) {
            if (typeDef->attributes[i].name == node.memberName) {
                memberIndex = i;
                break;
            }
        }
        
        if (memberIndex >= 0) {


            std::cerr << "Warning: MemberAccessNode implementation is incomplete" << std::endl;
            lastValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
        } else {
            std::cerr << "Error: Member '" << node.memberName << "' not found" << std::endl;
            lastValue = nullptr;
        }
    } else {
        std::cerr << "Error: Type definition not found for object" << std::endl;
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
    

    std::string objTypeName = "";
    
    if (auto varNode = dynamic_cast<VariableNode*>(node.object)) {

        auto typeIt = variableTypes.find(varNode->identifier);
        if (typeIt != variableTypes.end()) {
            objTypeName = typeIt->second;
        }
        std::cerr << "DEBUG: Variable '" << varNode->identifier << "' has type '" << objTypeName << "'" << std::endl;
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.object)) {
        objTypeName = typeInstNode->typeName;
        std::cerr << "DEBUG: Direct type instantiation of '" << objTypeName << "'" << std::endl;
    }
    

    std::vector<std::string> typesToTry;
    if (!objTypeName.empty()) {
        typesToTry.push_back(objTypeName);


        std::string currentParentName = objTypeName;
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

        for (const auto& typePair : types) {
            typesToTry.push_back(typePair.first);
        }
    }
    
    std::cerr << "DEBUG: Looking for method '" << node.methodName << "' in types: ";
    for (const auto& type : typesToTry) {
        std::cerr << type << " ";
    }
    std::cerr << std::endl;
    

    std::vector<llvm::Value*> args;
    args.push_back(objPtr);
    
    for (auto arg : node.arguments) {
        arg->accept(*this);
        if (lastValue) {
            args.push_back(lastValue);
        }
    }
    

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
    
    if (!methodFunc) {
        std::cerr << "Error: Method '" << node.methodName << "' not found for object type" << std::endl;
        lastValue = nullptr;
        return;
    }
    

    lastValue = builder.CreateCall(methodFunc, args, "method_result");
}