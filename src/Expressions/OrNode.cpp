#include "Expressions/OrNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

OrNode::OrNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}



char OrNode::getOperator() const {
    return '|';
}

void OrNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
