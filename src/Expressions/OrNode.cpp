#include "Expressions/OrNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

OrNode::OrNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int OrNode::evaluate() const {
    return left->evaluate() || right->evaluate();
}

char OrNode::getOperator() const {
    return '|';
}

void OrNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
