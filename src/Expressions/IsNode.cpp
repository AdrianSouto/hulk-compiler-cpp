#include "Expressions/IsNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Types/Type.hpp"
#include "Globals.hpp"
#include <iostream>

IsNode::~IsNode() {
    delete expression;
}

int IsNode::evaluate() const {
    
    
    return evaluateString() == "true" ? 1 : 0;
}

std::string IsNode::evaluateString() const {
    
    
    
    
    if (typeName == "Number") {
        
        if (dynamic_cast<NumberNode*>(expression)) {
            return "true";
        }
        
        return "false"; 
    }
    
    if (typeName == "String") {
        
        if (dynamic_cast<StringLiteralNode*>(expression)) {
            return "true";
        }
        
        return "false"; 
    }
    
    if (typeName == "Boolean") {
        
        if (dynamic_cast<BooleanNode*>(expression)) {
            return "true";
        }
        
        return "false"; 
    }
    
    
    if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(expression)) {
        
        Type* exprType = getTypeByName(typeInstNode->typeName);
        Type* targetType = getTypeByName(typeName);
        
        if (exprType && targetType) {
            
            
            bool result = exprType->conformsTo(targetType) || 
                         (exprType->getTypeName() == targetType->getTypeName());
            return result ? "true" : "false";
        }
    }
    
    
    if (dynamic_cast<VariableNode*>(expression)) {
        
        
        return "false"; 
    }
    
    
    
    return "false";
}

void IsNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Is: ";
    expression->print(0);
    std::cout << " is " << typeName << std::endl;
}

bool IsNode::validate(IContext* context) {
    
    if (!expression->validate(context)) {
        errorMessage = "Error in 'is' expression: " + expression->getErrorMessage();
        return false;
    }
    
    
    
    
    
    return true;
}

void IsNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}