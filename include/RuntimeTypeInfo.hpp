#ifndef RUNTIME_TYPE_INFO_HPP
#define RUNTIME_TYPE_INFO_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <string>
#include <map>

// Estructura TypeInfo como la describiste
struct TypeInfoData {
    const char* name;
    TypeInfoData* base_class;
};

// Clase para manejar el sistema de tipos en LLVM
class SimpleRuntimeTypeSystem {
public:
    static SimpleRuntimeTypeSystem& getInstance();
    
    // Inicializar el sistema
    void initialize(llvm::Module& module, llvm::LLVMContext& ctx);
    
    // Crear la estructura TypeInfo en LLVM
    llvm::StructType* getTypeInfoStructType(llvm::LLVMContext& ctx);
    
    // Crear la estructura Object en LLVM
    llvm::StructType* getObjectStructType(llvm::LLVMContext& ctx);
    
    // Crear TypeInfo global para un tipo específico
    llvm::GlobalVariable* createTypeInfoGlobal(const std::string& typeName, 
                                              const std::string& parentTypeName,
                                              llvm::Module& module, 
                                              llvm::LLVMContext& ctx);
    
    // Crear función is_instance_of
    llvm::Function* createIsInstanceOfFunction(llvm::Module& module, llvm::LLVMContext& ctx);
    
    // Crear función downcast
    llvm::Function* createDowncastFunction(llvm::Module& module, llvm::LLVMContext& ctx);
    
    // Obtener TypeInfo global para un tipo
    llvm::GlobalVariable* getTypeInfoGlobal(const std::string& typeName);
    
private:
    SimpleRuntimeTypeSystem() = default;
    
    std::map<std::string, llvm::GlobalVariable*> typeInfoGlobals;
    std::map<std::string, std::string> typeParents;
    
    llvm::StructType* typeInfoStructType = nullptr;
    llvm::StructType* objectStructType = nullptr;
    llvm::Function* isInstanceOfFunc = nullptr;
    llvm::Function* downcastFunc = nullptr;
    
    bool initialized = false;
};

#endif