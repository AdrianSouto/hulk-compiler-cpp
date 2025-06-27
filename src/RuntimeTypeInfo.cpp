#include "RuntimeTypeInfo.hpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <iostream>

SimpleRuntimeTypeSystem& SimpleRuntimeTypeSystem::getInstance() {
    static SimpleRuntimeTypeSystem instance;
    return instance;
}

void SimpleRuntimeTypeSystem::initialize(llvm::Module& module, llvm::LLVMContext& ctx) {
    if (initialized) return;
    
    // Crear estructura TypeInfo: { i8* name, TypeInfo* base_class }
    std::vector<llvm::Type*> typeInfoFields = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),  // name
        llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)   // base_class (será TypeInfo*)
    };
    
    typeInfoStructType = llvm::StructType::create(ctx, typeInfoFields, "TypeInfo");
    
    // Actualizar el segundo campo para que sea TypeInfo*
    typeInfoFields[1] = llvm::PointerType::get(typeInfoStructType, 0);
    typeInfoStructType->setBody(typeInfoFields);
    
    // Crear estructura Object: { TypeInfo* type_info, ... }
    std::vector<llvm::Type*> objectFields = {
        llvm::PointerType::get(typeInfoStructType, 0)  // type_info
    };
    
    objectStructType = llvm::StructType::create(ctx, objectFields, "Object");
    
    initialized = true;
    std::cerr << "DEBUG: SimpleRuntimeTypeSystem initialized" << std::endl;
}

llvm::StructType* SimpleRuntimeTypeSystem::getTypeInfoStructType(llvm::LLVMContext& ctx) {
    if (!typeInfoStructType) {
        // Crear estructura temporal para inicialización
        std::vector<llvm::Type*> typeInfoFields = {
            llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),  // name
            llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)   // base_class (temporal)
        };
        typeInfoStructType = llvm::StructType::create(ctx, typeInfoFields, "TypeInfo");
    }
    return typeInfoStructType;
}

llvm::StructType* SimpleRuntimeTypeSystem::getObjectStructType(llvm::LLVMContext& ctx) {
    if (!objectStructType) {
        getTypeInfoStructType(ctx); // Asegurar que TypeInfo existe
        std::vector<llvm::Type*> objectFields = {
            llvm::PointerType::get(typeInfoStructType, 0)  // type_info
        };
        objectStructType = llvm::StructType::create(ctx, objectFields, "Object");
    }
    return objectStructType;
}

llvm::GlobalVariable* SimpleRuntimeTypeSystem::createTypeInfoGlobal(const std::string& typeName, 
                                                                    const std::string& parentTypeName,
                                                                    llvm::Module& module, 
                                                                    llvm::LLVMContext& ctx) {
    // Verificar si ya existe
    auto it = typeInfoGlobals.find(typeName);
    if (it != typeInfoGlobals.end()) {
        return it->second;
    }
    
    // Guardar relación padre
    typeParents[typeName] = parentTypeName;
    
    // Crear string global para el nombre del tipo
    llvm::Constant* typeNameStr = llvm::ConstantDataArray::getString(ctx, typeName);
    llvm::GlobalVariable* typeNameGlobal = new llvm::GlobalVariable(
        module, typeNameStr->getType(), true, llvm::GlobalValue::PrivateLinkage,
        typeNameStr, typeName + "_name_str");
    
    // Obtener puntero al TypeInfo del padre (si existe)
    llvm::Constant* parentTypeInfoPtr = llvm::ConstantPointerNull::get(
        llvm::PointerType::get(typeInfoStructType, 0));
    
    if (!parentTypeName.empty() && parentTypeName != typeName) {
        // Si el padre ya existe, usar su TypeInfo
        auto parentIt = typeInfoGlobals.find(parentTypeName);
        if (parentIt != typeInfoGlobals.end()) {
            parentTypeInfoPtr = parentIt->second;
        }
        // Si no existe, se creará después y se actualizará
    }
    
    // Crear estructura TypeInfo
    std::vector<llvm::Constant*> typeInfoValues = {
        llvm::ConstantExpr::getBitCast(typeNameGlobal, llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)),
        parentTypeInfoPtr
    };
    
    llvm::Constant* typeInfoStruct = llvm::ConstantStruct::get(typeInfoStructType, typeInfoValues);
    
    // Crear variable global para TypeInfo
    llvm::GlobalVariable* typeInfoGlobal = new llvm::GlobalVariable(
        module, typeInfoStructType, true, llvm::GlobalValue::ExternalLinkage,
        typeInfoStruct, typeName + "_typeinfo");
    
    typeInfoGlobals[typeName] = typeInfoGlobal;

    return typeInfoGlobal;
}

llvm::Function* SimpleRuntimeTypeSystem::createIsInstanceOfFunction(llvm::Module& module, llvm::LLVMContext& ctx) {
    if (isInstanceOfFunc) {
        return isInstanceOfFunc;
    }
    
    // Función: int is_instance_of(Object* obj, TypeInfo* target_type)
    std::vector<llvm::Type*> paramTypes = {
        llvm::PointerType::get(objectStructType, 0),      // Object* obj
        llvm::PointerType::get(typeInfoStructType, 0)     // TypeInfo* target_type
    };
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(ctx), paramTypes, false);
    
    isInstanceOfFunc = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, "is_instance_of", module);
    
    // Crear bloques básicos
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(ctx, "entry", isInstanceOfFunc);
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(ctx, "loop", isInstanceOfFunc);
    llvm::BasicBlock* checkBB = llvm::BasicBlock::Create(ctx, "check", isInstanceOfFunc);
    llvm::BasicBlock* matchBB = llvm::BasicBlock::Create(ctx, "match", isInstanceOfFunc);
    llvm::BasicBlock* nextBB = llvm::BasicBlock::Create(ctx, "next", isInstanceOfFunc);
    llvm::BasicBlock* noMatchBB = llvm::BasicBlock::Create(ctx, "no_match", isInstanceOfFunc);
    llvm::BasicBlock* returnTrueBB = llvm::BasicBlock::Create(ctx, "return_true", isInstanceOfFunc);
    
    llvm::IRBuilder<> builder(entryBB);
    
    auto args = isInstanceOfFunc->arg_begin();
    llvm::Value* objPtr = &*args++;
    llvm::Value* targetTypePtr = &*args;
    
    // Verificar si obj es NULL
    llvm::Value* objIsNull = builder.CreateIsNull(objPtr, "obj_is_null");
    builder.CreateCondBr(objIsNull, noMatchBB, loopBB);
    
    // Obtener type_info del objeto
    builder.SetInsertPoint(loopBB);
    llvm::Value* objTypeInfoPtr = builder.CreateStructGEP(objectStructType, objPtr, 0, "obj_typeinfo_ptr");
    llvm::Value* currentTypeInfo = builder.CreateLoad(
        llvm::PointerType::get(typeInfoStructType, 0), objTypeInfoPtr, "current_typeinfo");
    
    builder.CreateBr(checkBB);
    
    // Loop para recorrer la jerarquía
    builder.SetInsertPoint(checkBB);
    llvm::PHINode* currentTypePhi = builder.CreatePHI(
        llvm::PointerType::get(typeInfoStructType, 0), 2, "current_type");
    currentTypePhi->addIncoming(currentTypeInfo, loopBB);
    
    // Verificar si current es NULL
    llvm::Value* currentIsNull = builder.CreateIsNull(currentTypePhi, "current_is_null");
    builder.CreateCondBr(currentIsNull, noMatchBB, matchBB);
    
    // Comparar tipos
    builder.SetInsertPoint(matchBB);
    llvm::Value* typesMatch = builder.CreateICmpEQ(currentTypePhi, targetTypePtr, "types_match");
    builder.CreateCondBr(typesMatch, returnTrueBB, nextBB);
    
    // Obtener tipo padre
    builder.SetInsertPoint(nextBB);
    llvm::Value* baseClassPtr = builder.CreateStructGEP(typeInfoStructType, currentTypePhi, 1, "base_class_ptr");
    llvm::Value* baseClass = builder.CreateLoad(
        llvm::PointerType::get(typeInfoStructType, 0), baseClassPtr, "base_class");
    
    currentTypePhi->addIncoming(baseClass, nextBB);
    builder.CreateBr(checkBB);
    
    // Retornar true
    builder.SetInsertPoint(returnTrueBB);
    builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1));
    
    // Retornar false
    builder.SetInsertPoint(noMatchBB);
    builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0));
    
    std::cerr << "DEBUG: Created is_instance_of function" << std::endl;
    
    return isInstanceOfFunc;
}

llvm::Function* SimpleRuntimeTypeSystem::createDowncastFunction(llvm::Module& module, llvm::LLVMContext& ctx) {
    if (downcastFunc) {
        return downcastFunc;
    }
    
    // Función: Object* downcast(Object* base_obj, TypeInfo* target_type)
    std::vector<llvm::Type*> paramTypes = {
        llvm::PointerType::get(objectStructType, 0),      // Object* base_obj
        llvm::PointerType::get(typeInfoStructType, 0)     // TypeInfo* target_type
    };
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::PointerType::get(objectStructType, 0), paramTypes, false);
    
    downcastFunc = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, "downcast", module);
    
    // Crear bloques básicos
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(ctx, "entry", downcastFunc);
    llvm::BasicBlock* validBB = llvm::BasicBlock::Create(ctx, "valid", downcastFunc);
    llvm::BasicBlock* invalidBB = llvm::BasicBlock::Create(ctx, "invalid", downcastFunc);
    
    llvm::IRBuilder<> builder(entryBB);
    
    auto args = downcastFunc->arg_begin();
    llvm::Value* baseObjPtr = &*args++;
    llvm::Value* targetTypePtr = &*args;
    
    // Llamar a is_instance_of
    llvm::Function* isInstanceFunc = createIsInstanceOfFunction(module, ctx);
    llvm::Value* isValid = builder.CreateCall(isInstanceFunc, {baseObjPtr, targetTypePtr}, "is_valid");
    
    // Verificar resultado
    llvm::Value* isValidBool = builder.CreateICmpNE(
        isValid, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0), "is_valid_bool");
    builder.CreateCondBr(isValidBool, validBB, invalidBB);
    
    // Cast válido
    builder.SetInsertPoint(validBB);
    builder.CreateRet(baseObjPtr);
    
    // Cast inválido
    builder.SetInsertPoint(invalidBB);
    
    // Obtener printf para error
    llvm::Function* printfFunc = module.getFunction("printf");
    if (!printfFunc) {
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(ctx),
            {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
            true
        );
        printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);
    }
    
    // Imprimir error y retornar NULL
    llvm::Value* errorStr = builder.CreateGlobalStringPtr("Runtime error: Invalid downcast\\n", "downcast_error");
    builder.CreateCall(printfFunc, {errorStr});
    builder.CreateRet(llvm::ConstantPointerNull::get(llvm::PointerType::get(objectStructType, 0)));
    
    std::cerr << "DEBUG: Created downcast function" << std::endl;
    
    return downcastFunc;
}

llvm::GlobalVariable* SimpleRuntimeTypeSystem::getTypeInfoGlobal(const std::string& typeName) {
    auto it = typeInfoGlobals.find(typeName);
    if (it != typeInfoGlobals.end()) {
        return it->second;
    }
    return nullptr;
}