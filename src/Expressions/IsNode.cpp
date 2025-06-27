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