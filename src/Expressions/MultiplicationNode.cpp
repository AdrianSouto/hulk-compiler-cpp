#include "Expressions/MultiplicationNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

MultiplicationNode::MultiplicationNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}


char MultiplicationNode::getOperator() const {
    return '*';
}

void MultiplicationNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

