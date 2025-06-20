#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <map>
#include <string>
#include <vector>

// Forward declarations
class ExpressionNode;
class TypeDefNode;
class Type;

// LLVM forward declarations
namespace llvm {
    class StructType;
}

extern std::map<std::string, std::string> variables;
extern std::map<std::string, std::pair<std::vector<std::string>, ExpressionNode*>> functions;

extern std::map<std::string, TypeDefNode*> types;

extern std::map<std::string, Type*> typeRegistry;

extern std::map<std::string, std::string> variableTypes;

// Global map for LLVM struct types
extern std::map<std::string, llvm::StructType*> structTypes;

Type* getTypeByName(const std::string& typeName);
void registerType(const std::string& typeName, Type* type);
Type* createUserDefinedType(const std::string& typeName, TypeDefNode* typeDef, const std::string& parentTypeName = "");

#endif 