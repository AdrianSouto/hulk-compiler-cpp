#include "../../include/Expressions/PrintExpressionNode.hpp"
#include "../../include/Visitors/LLVMCodegenVisitor.hpp"
#include "../../include/Context/IContext.hpp"
#include <iostream>

PrintExpressionNode::PrintExpressionNode(ExpressionNode* expr) : expression(expr) {}

PrintExpressionNode::~PrintExpressionNode() {
    delete expression;
}

void PrintExpressionNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}


bool PrintExpressionNode::validate(IContext* context) {
    if (!expression) {
        setErrorMessage("Print expression has no expression to print");
        return false;
    }
    
    if (!expression->validate(context)) {
        setErrorMessage("Error in print expression: " + expression->getErrorMessage());
        return false;
    }
    
    return true;
}

