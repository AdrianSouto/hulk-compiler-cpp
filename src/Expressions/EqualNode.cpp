#include "Expressions/EqualNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

EqualNode::EqualNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}



char EqualNode::getOperator() const {
    return '=';
}

void EqualNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
