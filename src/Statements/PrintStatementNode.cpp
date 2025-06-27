#include "Statements/PrintStatementNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include <iostream>

PrintStatementNode::PrintStatementNode(ExpressionNode* expr) : expression(expr) {}


bool PrintStatementNode::validate(IContext* context) {
    if (!expression->validate(context)) {
        errorMessage = "Error in print statement: " + expression->getErrorMessage();
        return false;
    }
    return true;
}

void PrintStatementNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

PrintStatementNode::~PrintStatementNode() {
    delete expression;
}

