#include "Expressions/AndNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

AndNode::AndNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int AndNode::evaluate() const {
    return left->evaluate() && right->evaluate();
}

char AndNode::getOperator() const {
    return '&';
}

void AndNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
