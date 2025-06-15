#include "Expressions/UnaryMinusNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

UnaryMinusNode::UnaryMinusNode(ExpressionNode* operand)
    : UnaryOperatorNode(operand) {}

int UnaryMinusNode::evaluate() const {
    return -operand->evaluate();
}

double UnaryMinusNode::evaluateDouble() const {
    return -operand->evaluateDouble();
}

char UnaryMinusNode::getOperator() const {
    return '-';
}

void UnaryMinusNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}