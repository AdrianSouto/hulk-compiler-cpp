#include "Expressions\NotEqualNode.hpp"
#include "Visitors\LLVMCodegenVisitor.hpp"

NotEqualNode::NotEqualNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}


char NotEqualNode::getOperator() const {
    return '!';
}

void NotEqualNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
