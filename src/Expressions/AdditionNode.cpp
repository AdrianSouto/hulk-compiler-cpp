#include "Expressions/AdditionNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

AdditionNode::AdditionNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}


char AdditionNode::getOperator() const {
    return '+';
}

void AdditionNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

