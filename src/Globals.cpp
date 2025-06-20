#include "Globals.hpp"
#include "AST/ExpressionNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Types/Type.hpp"

#include <llvm/IR/DerivedTypes.h>

std::map<std::string, std::string> variables;
std::map<std::string, std::pair<std::vector<std::string>, ExpressionNode*>> functions;
std::map<std::string, TypeDefNode*> types;
std::map<std::string, Type*> typeRegistry;
std::map<std::string, std::string> variableTypes;

// Global map for LLVM struct types
std::map<std::string, llvm::StructType*> structTypes;

static bool typeRegistryInitialized = false;

void initializeTypeRegistry() {
    if (typeRegistryInitialized) return;
    
    
    typeRegistry["Object"] = Type::getObjectType();
    typeRegistry["Number"] = Type::getNumberType();
    typeRegistry["String"] = Type::getStringType();
    typeRegistry["Boolean"] = Type::getBooleanType();
    typeRegistry["Void"] = Type::getVoidType();
    typeRegistry["Unknown"] = Type::getUnknownType();
    
    typeRegistryInitialized = true;
}

Type* getTypeByName(const std::string& typeName) {
    initializeTypeRegistry();
    
    auto it = typeRegistry.find(typeName);
    if (it != typeRegistry.end()) {
        return it->second;
    }
    
    
    return Type::getUnknownType();
}

void registerType(const std::string& typeName, Type* type) {
    initializeTypeRegistry();
    typeRegistry[typeName] = type;
}

Type* createUserDefinedType(const std::string& typeName, TypeDefNode* typeDef, const std::string& parentTypeName) {
    initializeTypeRegistry();
    
    
    Type* parentType = nullptr;
    if (!parentTypeName.empty()) {
        parentType = getTypeByName(parentTypeName);
        if (parentType == Type::getUnknownType()) {
            
            parentType = Type::getObjectType();
        }
    } else {
        parentType = Type::getObjectType(); 
    }
    
    
    Type* newType = Type::createUserDefinedType(typeName, typeDef, parentType);
    
    
    registerType(typeName, newType);
    
    return newType;
}

