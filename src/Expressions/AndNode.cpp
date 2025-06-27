#include "Expressions/AndNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

AndNode::AndNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}


char AndNode::getOperator() const {
    return '&';
}

void AndNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
