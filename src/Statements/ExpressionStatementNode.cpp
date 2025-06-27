#include "Statements/ExpressionStatementNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include <iostream>

ExpressionStatementNode::ExpressionStatementNode(ExpressionNode* expr)
    : expression(expr) {}



bool ExpressionStatementNode::validate(IContext* context) {
    if (expression) {
        bool result = expression->validate(context);
        if (!result) {
            errorMessage = expression->getErrorMessage();
        }
        return result;
    }
    return true;
}

void ExpressionStatementNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

ExpressionStatementNode::~ExpressionStatementNode() {
    delete expression;
}