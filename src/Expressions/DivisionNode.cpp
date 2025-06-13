#include "Expressions/DivisionNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

DivisionNode::DivisionNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int DivisionNode::evaluate() const {
    return left->evaluate() / right->evaluate();
}

char DivisionNode::getOperator() const {
    return '/';
}

void DivisionNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

