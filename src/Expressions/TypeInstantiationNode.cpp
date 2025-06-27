#include "Expressions/TypeInstantiationNode.hpp"
#include "Context/IContext.hpp"
#include "Context/Context.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Types/Type.hpp"
#include "Globals.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

TypeInstantiationNode::~TypeInstantiationNode() {
    for (auto arg : arguments) {
        delete arg;
    }
}

bool TypeInstantiationNode::validate(IContext* context) {
    // First validate all arguments
    for (size_t i = 0; i < arguments.size(); i++) {
        if (!arguments[i]->validate(context)) {
            errorMessage = "Error in argument " + std::to_string(i+1) + " of type '" + 
                           typeName + "': " + arguments[i]->getErrorMessage();
            return false;
        }
    }
    
    // Check if the type exists
    auto typeIt = types.find(typeName);
    if (typeIt == types.end()) {
        errorMessage = "Error: Type '" + typeName + "' is not defined";
        return false;
    }
    
    TypeDefNode* typeDef = typeIt->second;
    
    // Get the constructor parameters - if this type doesn't define any, check parent
    std::vector<Parameter>* constructorParams = &typeDef->typeArguments;
    TypeDefNode* currentTypeDef = typeDef;
    
    // If this type has no constructor parameters but has a parent, use parent's parameters
    while (constructorParams->empty() && !currentTypeDef->parentTypeName.empty()) {
        auto parentIt = types.find(currentTypeDef->parentTypeName);
        if (parentIt != types.end()) {
            currentTypeDef = parentIt->second;
            constructorParams = &currentTypeDef->typeArguments;
        } else {
            break;
        }
    }
    
    // Check number of arguments
    if (arguments.size() != constructorParams->size()) {
        errorMessage = "Error: Type '" + typeName + "' expects " + 
                       std::to_string(constructorParams->size()) + 
                       " arguments but got " + std::to_string(arguments.size());
        return false;
    }
    
    // Check type of each argument
    for (size_t i = 0; i < arguments.size(); i++) {
        Type* expectedType = (*constructorParams)[i].type;
        Type* actualType = arguments[i]->inferType(context);
        
        if (expectedType && actualType && expectedType->getTypeName() != "Unknown") {
            if (!actualType->conformsTo(expectedType)) {
                errorMessage = "Error: Constructor of type '" + typeName + 
                               "' expects argument " + std::to_string(i+1) + 
                               " ('" + (*constructorParams)[i].name + "') to be of type " + 
                               expectedType->getTypeName() + " but got " + 
                               actualType->getTypeName();
                return false;
            }
        }
    }
    
    return true;
}

Type* TypeInstantiationNode::inferType(IContext* context) const {
    (void)context; // Suppress unused parameter warning
    
    // Return the type being instantiated
    Type* type = getTypeByName(typeName);
    if (type && type != Type::getUnknownType()) {
        return type;
    }
    
    // If type not found, return Unknown
    return Type::getUnknownType();
}

void TypeInstantiationNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}