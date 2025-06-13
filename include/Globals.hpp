#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <map>
#include <string>
#include <vector>


class ExpressionNode;
class TypeDefNode;
class Type;


extern std::map<std::string, std::string> variables;
extern std::map<std::string, std::pair<std::vector<std::string>, ExpressionNode*>> functions;


extern std::map<std::string, TypeDefNode*> types;


extern std::map<std::string, Type*> typeRegistry;


extern std::map<std::string, std::string> variableTypes;


Type* getTypeByName(const std::string& typeName);
void registerType(const std::string& typeName, Type* type);
Type* createUserDefinedType(const std::string& typeName, TypeDefNode* typeDef, const std::string& parentTypeName = "");

#endif 

