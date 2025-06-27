#include "Expressions/SubtractionNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

SubtractionNode::SubtractionNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

char SubtractionNode::getOperator() const {
    return '-';
}

void SubtractionNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

