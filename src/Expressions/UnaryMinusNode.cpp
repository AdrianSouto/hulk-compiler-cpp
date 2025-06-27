#include "Expressions/UnaryMinusNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

UnaryMinusNode::UnaryMinusNode(ExpressionNode* operand)
    : UnaryOperatorNode(operand) {}



char UnaryMinusNode::getOperator() const {
    return '-';
}

void UnaryMinusNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}