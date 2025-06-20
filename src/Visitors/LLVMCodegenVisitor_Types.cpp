#include "Visitors/LLVMCodegenVisitor.hpp"
#include "RuntimeTypeInfo.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/IsNode.hpp"
#include "Expressions/AsNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <iostream>

// Mapa global para estructuras de tipos
extern std::map<std::string, llvm::StructType*> structTypes;

static llvm::Type* getLLVMTypeFromName(const std::string& typeName, llvm::LLVMContext& ctx) {
    if (typeName == "Number") {
        return llvm::Type::getDoubleTy(ctx);
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
    SimpleRuntimeTypeSystem& rts = SimpleRuntimeTypeSystem::getInstance();
    rts.initialize(module, ctx);
    
    std::cerr << "DEBUG: Processing TypeDefNode for '" << node.typeName << "'" << std::endl;
    
    // Crear TypeInfo global para este tipo
    llvm::GlobalVariable* typeInfoGlobal = rts.createTypeInfoGlobal(
        node.typeName, node.parentTypeName, module, ctx);
    
    // Recopilar tipos de miembros
    std::vector<llvm::Type*> memberTypes;
    std::vector<std::string> memberNames;

    // Función para agregar atributos heredados
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
                llvm::Type* attrType = ::getLLVMTypeFromName(attrTypeName, ctx);
                memberTypes.push_back(attrType);
                memberNames.push_back(attr.name);
            }
        }
    };

    // Agregar atributos heredados
    if (!node.parentTypeName.empty()) {
        addInheritedAttributes(node.parentTypeName);
    }

    // Agregar atributos propios
    for (const auto& attr : node.attributes) {
        std::string typeName = "Number";
        if (attr.type) {
            typeName = attr.type->toString();
        }
        llvm::Type* attrType = ::getLLVMTypeFromName(typeName, ctx);
        memberTypes.push_back(attrType);
        memberNames.push_back(attr.name);
    }

    // Crear estructura de objeto: Object + miembros
    std::vector<llvm::Type*> objectFields;
    
    // Primer campo: TypeInfo*
    objectFields.push_back(llvm::PointerType::get(rts.getTypeInfoStructType(ctx), 0));
    
    // Agregar campos de miembros
    for (llvm::Type* memberType : memberTypes) {
        objectFields.push_back(memberType);
    }

    // Crear estructura del tipo
    llvm::StructType* structType = llvm::StructType::create(ctx, objectFields, node.typeName);
    structTypes[node.typeName] = structType;

    // Procesar métodos (igual que antes)
    for (auto method : node.methods) {
        if (auto defFunc = dynamic_cast<DefFuncNode*>(method)) {
            std::string originalName = defFunc->identifier;
            defFunc->identifier = node.typeName + "_" + originalName;

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

    // Crear constructor mejorado
    createSimpleConstructor(node, structType, typeInfoGlobal);
    
    lastValue = nullptr;
}

void LLVMCodegenVisitor::createSimpleConstructor(TypeDefNode& node, 
                                                 llvm::StructType* structType, 
                                                 llvm::GlobalVariable* typeInfoGlobal) {
    std::string constructorName = "new_" + node.typeName;
    std::vector<llvm::Type*> constructorParamTypes;

    // Obtener argumentos de tipo efectivos
    std::vector<Parameter> effectiveTypeArguments = node.typeArguments;

    if (effectiveTypeArguments.empty() && !node.parentTypeName.empty() && node.parentArgs.empty()) {
        std::string currentParent = node.parentTypeName;
        while (!currentParent.empty() && effectiveTypeArguments.empty()) {
            auto parentTypeIt = types.find(currentParent);
            if (parentTypeIt != types.end()) {
                effectiveTypeArguments = parentTypeIt->second->typeArguments;
                currentParent = parentTypeIt->second->parentTypeName;
            } else {
                break;
            }
        }
    }

    // Construir tipos de parámetros del constructor
    for (const auto& param : effectiveTypeArguments) {
        std::string typeName = "Number";
        if (param.type) {
            typeName = param.type->toString();
        }
        llvm::Type* paramType = ::getLLVMTypeFromName(typeName, ctx);
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

    // Asignar memoria para el objeto
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

    // CRUCIAL: Inicializar el campo TypeInfo* (índice 0)
    llvm::Value* typeInfoField = constructorBuilder.CreateStructGEP(structType, typedPtr, 0, "typeinfo_field");
    constructorBuilder.CreateStore(typeInfoGlobal, typeInfoField);

    // Inicializar otros miembros (empezando desde índice 1)
    std::map<std::string, llvm::Value*> constructorParams;
    auto paramIt = constructorFunc->arg_begin();

    for (const auto& param : effectiveTypeArguments) {
        if (paramIt != constructorFunc->arg_end()) {
            constructorParams[param.name] = &*paramIt;
            ++paramIt;
        }
    }

    // Inicializar miembros (lógica similar a la anterior pero empezando desde índice 1)
    size_t memberIndex = 1; // Saltar el campo TypeInfo*
    
    // Construir cadena de herencia
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

    // Inicializar miembros
    for (size_t chainIndex = 0; chainIndex < inheritanceChain.size(); ++chainIndex) {
        TypeDefNode* chainTypeDef = inheritanceChain[chainIndex];

        for (size_t i = 0; i < chainTypeDef->attributes.size(); ++i) {
            llvm::Value* memberPtr = constructorBuilder.CreateStructGEP(structType, typedPtr, memberIndex);
            llvm::Value* initValue = nullptr;

            // Buscar valor de inicialización
            if (i < chainTypeDef->typeArguments.size() && 
                chainTypeDef->typeArguments[i].name == chainTypeDef->attributes[i].name) {
                auto paramIt = constructorParams.find(chainTypeDef->typeArguments[i].name);
                if (paramIt != constructorParams.end()) {
                    initValue = paramIt->second;
                }
            }

            // Valor por defecto si no se encuentra inicialización
            if (!initValue) {
                std::string attrTypeName = "Number";
                if (chainTypeDef->attributes[i].type) {
                    attrTypeName = chainTypeDef->attributes[i].type->toString();
                }

                if (attrTypeName == "Number") {
                    initValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), 0.0);
                } else if (attrTypeName == "Boolean") {
                    initValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), 0);
                } else {
                    initValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
                }
            }

            constructorBuilder.CreateStore(initValue, memberPtr);
            memberIndex++;
        }
    }

    constructorBuilder.CreateRet(typedPtr);
    
    std::cerr << "DEBUG: Created simple constructor for '" << node.typeName << "'" << std::endl;
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

    // Evaluar argumentos
    std::vector<llvm::Value*> args;
    auto paramIt = constructorFunc->arg_begin();
    for (auto arg : node.arguments) {
        arg->accept(*this);
        if (lastValue && paramIt != constructorFunc->arg_end()) {
            args.push_back(lastValue);
            ++paramIt;
        } else if (lastValue) {
            args.push_back(lastValue);
        }
    }

    // Llamar al constructor
    lastValue = builder.CreateCall(constructorFunc, args, "new_instance");

    // Almacenar el tipo real instanciado
    variableTypes["temp_instance"] = node.typeName;
    
    std::cerr << "DEBUG: Created instance of type '" << node.typeName << "' with TypeInfo" << std::endl;
}

void LLVMCodegenVisitor::visit(IsNode& node) {
    SimpleRuntimeTypeSystem& rts = SimpleRuntimeTypeSystem::getInstance();
    rts.initialize(module, ctx);
    
    node.expression->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    if (!exprValue) {
        std::cerr << "Error: Invalid expression in 'is' check" << std::endl;
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), 0);
        return;
    }

    // Obtener TypeInfo global del tipo objetivo
    llvm::GlobalVariable* targetTypeInfo = rts.getTypeInfoGlobal(node.typeName);
    if (!targetTypeInfo) {
        std::cerr << "Error: TypeInfo not found for '" << node.typeName << "'" << std::endl;
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), 0);
        return;
    }

    // Manejar tipos built-in
    std::string exprTypeName = "";
    if (auto varNode = dynamic_cast<VariableNode*>(node.expression)) {
        auto typeIt = variableTypes.find(varNode->identifier);
        if (typeIt != variableTypes.end()) {
            exprTypeName = typeIt->second;
        }
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.expression)) {
        exprTypeName = typeInstNode->typeName;
    }

    if (exprTypeName == "Number" || exprTypeName == "Boolean" || exprTypeName == "String") {
        // Verificación en tiempo de compilación para tipos built-in
        bool isMatch = (exprTypeName == node.typeName) || (node.typeName == "Object");
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), isMatch ? 1 : 0);
        return;
    }

    // Para tipos personalizados, usar verificación en tiempo de ejecución
    if (exprValue->getType()->isPointerTy()) {
        // Convertir a Object*
        llvm::Value* objPtr = builder.CreateBitCast(
            exprValue, llvm::PointerType::get(rts.getObjectStructType(ctx), 0), "obj_ptr");
        
        // Llamar a is_instance_of
        llvm::Function* isInstanceFunc = rts.createIsInstanceOfFunction(module, ctx);
        llvm::Value* result = builder.CreateCall(isInstanceFunc, {objPtr, targetTypeInfo}, "is_instance_result");
        
        // Convertir resultado int a bool
        lastValue = builder.CreateICmpNE(
            result, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0), "is_result_bool");
        
        std::cerr << "DEBUG: Runtime IsNode check for '" << node.typeName << "'" << std::endl;
    } else {
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), 0);
    }
}

void LLVMCodegenVisitor::visit(AsNode& node) {
    SimpleRuntimeTypeSystem& rts = SimpleRuntimeTypeSystem::getInstance();
    rts.initialize(module, ctx);
    
    node.expression->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    if (!exprValue) {
        std::cerr << "Error: Invalid expression in 'as' cast" << std::endl;
        lastValue = nullptr;
        return;
    }

    // Obtener TypeInfo global del tipo objetivo
    llvm::GlobalVariable* targetTypeInfo = rts.getTypeInfoGlobal(node.typeName);
    if (!targetTypeInfo) {
        std::cerr << "Error: TypeInfo not found for '" << node.typeName << "'" << std::endl;
        lastValue = nullptr;
        return;
    }

    // Manejar tipos built-in
    std::string exprTypeName = "";
    if (auto varNode = dynamic_cast<VariableNode*>(node.expression)) {
        auto typeIt = variableTypes.find(varNode->identifier);
        if (typeIt != variableTypes.end()) {
            exprTypeName = typeIt->second;
        }
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.expression)) {
        exprTypeName = typeInstNode->typeName;
    }

    if (exprTypeName == "Number" || exprTypeName == "Boolean" || exprTypeName == "String") {
        // Cast de tipos built-in (igual que antes)
        bool canCast = (exprTypeName == node.typeName) || (node.typeName == "Object");
        
        if (canCast) {
            lastValue = exprValue;
        } else {
            // Generar error
            llvm::Function* printfFunc = module.getFunction("printf");
            if (!printfFunc) {
                llvm::FunctionType* printfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx),
                    {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
                    true
                );
                printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);
            }
            
            std::string errorMsg = "Runtime error: Cannot cast " + exprTypeName + " to " + node.typeName + "\\n";
            llvm::Value* errorStr = builder.CreateGlobalStringPtr(errorMsg, "cast_error_msg");
            builder.CreateCall(printfFunc, {errorStr});
            
            // Fix: Properly cast to PointerType
            if (exprValue->getType()->isPointerTy()) {
                lastValue = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(exprValue->getType()));
            } else {
                lastValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
            }
        }
        return;
    }

    // Para tipos personalizados, usar downcasting en tiempo de ejecución
    if (exprValue->getType()->isPointerTy()) {
        // Convertir a Object*
        llvm::Value* objPtr = builder.CreateBitCast(
            exprValue, llvm::PointerType::get(rts.getObjectStructType(ctx), 0), "obj_ptr");
        
        // Llamar a downcast
        llvm::Function* downcastFunc = rts.createDowncastFunction(module, ctx);
        llvm::Value* castResult = builder.CreateCall(downcastFunc, {objPtr, targetTypeInfo}, "downcast_result");
        
        // Convertir resultado de vuelta al tipo apropiado
        auto targetStructIt = structTypes.find(node.typeName);
        if (targetStructIt != structTypes.end()) {
            llvm::Type* targetType = llvm::PointerType::get(targetStructIt->second, 0);
            lastValue = builder.CreateBitCast(castResult, targetType, "final_cast");
        } else {
            lastValue = castResult;
        }
        
        std::cerr << "DEBUG: Runtime AsNode cast to '" << node.typeName << "'" << std::endl;
    } else {
        // Fix: Properly cast to PointerType
        if (exprValue->getType()->isPointerTy()) {
            lastValue = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(exprValue->getType()));
        } else {
            lastValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
        }
    }
}