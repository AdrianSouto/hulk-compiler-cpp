#include "Expressions/MultiplicationNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

MultiplicationNode::MultiplicationNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int MultiplicationNode::evaluate() const {
    return left->evaluate() * right->evaluate();
}

char MultiplicationNode::getOperator() const {
    return '*';
}

void MultiplicationNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

