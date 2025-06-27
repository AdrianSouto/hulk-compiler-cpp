#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <map>
#include <string>
#include <vector>

// Forward declarations
class TypeDefNode;
class Type;

// LLVM forward declarations
namespace llvm {
    class StructType;
}

// Type definitions registry
extern std::map<std::string, TypeDefNode*> types;

// Type system registry
extern std::map<std::string, Type*> typeRegistry;

// Global map for LLVM struct types
extern std::map<std::string, llvm::StructType*> structTypes;

// Type management functions
Type* getTypeByName(const std::string& typeName);
void registerType(const std::string& typeName, Type* type);
Type* createUserDefinedType(const std::string& typeName, TypeDefNode* typeDef, const std::string& parentTypeName = "");

#endif 