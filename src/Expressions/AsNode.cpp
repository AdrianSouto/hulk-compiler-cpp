#include "Expressions/AsNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Types/Type.hpp"
#include "Globals.hpp"
#include <iostream>
#include <stdexcept>

AsNode::~AsNode() {
    delete expression;
}

int AsNode::evaluate() const {
    
    
    return expression->evaluate();
}

std::string AsNode::evaluateString() const {
    
    
    
    
    
    
    
    if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(expression)) {
        
        Type* exprType = getTypeByName(typeInstNode->typeName);
        Type* targetType = getTypeByName(typeName);
        
        if (exprType && targetType) {
            
            
            bool canCast = exprType->conformsTo(targetType) || 
                          targetType->conformsTo(exprType) ||
                          (exprType->getTypeName() == targetType->getTypeName());
            if (canCast) {
                return expression->evaluateString();
            } else {
                throw std::runtime_error("Runtime error: Cannot cast " + typeInstNode->typeName + " to " + typeName);
            }
        }
    }
    
    
    if (dynamic_cast<VariableNode*>(expression)) {
        
        
        return expression->evaluateString();
    }
    
    
    
    return expression->evaluateString();
}

void AsNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "As: ";
    expression->print(0);
    std::cout << " as " << typeName << std::endl;
}

bool AsNode::validate(IContext* context) {
    
    if (!expression->validate(context)) {
        errorMessage = "Error in 'as' expression: " + expression->getErrorMessage();
        return false;
    }
    
    
    
    
    
    return true;
}

Type* AsNode::inferType(IContext* context) const {
    // The result type of an 'as' cast is the target type
    Type* targetType = nullptr;
    
    // Try to get the type from the global type registry
    if (typeName == "Number") {
        targetType = Type::getNumberType();
    } else if (typeName == "String") {
        targetType = Type::getStringType();
    } else if (typeName == "Boolean") {
        targetType = Type::getBooleanType();
    } else if (typeName == "Object") {
        targetType = Type::getObjectType();
    } else {
        // User-defined type
        targetType = getTypeByName(typeName);
        if (!targetType) {
            targetType = Type::getUnknownType();
        }
    }
    
    return targetType;
}

void AsNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}