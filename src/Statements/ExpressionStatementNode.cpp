#include "Statements/ExpressionStatementNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include <iostream>

ExpressionStatementNode::ExpressionStatementNode(ExpressionNode* expr)
    : expression(expr) {}

void ExpressionStatementNode::execute() const {
    
    if (expression) {
        expression->evaluate();
    }
}

void ExpressionStatementNode::print(int indent) const {
    for (int i = 0; i < indent; i++) std::cout << "  ";
    std::cout << "ExpressionStatement:" << std::endl;
    if (expression) {
        expression->print(indent + 1);
    }
}

bool ExpressionStatementNode::validate(IContext* context) {
    if (expression) {
        return expression->validate(context);
    }
    return true;
}

void ExpressionStatementNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

ExpressionStatementNode::~ExpressionStatementNode() {
    delete expression;
}