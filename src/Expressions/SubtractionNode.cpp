#include "Expressions/SubtractionNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

SubtractionNode::SubtractionNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int SubtractionNode::evaluate() const {
    return left->evaluate() - right->evaluate();
}

char SubtractionNode::getOperator() const {
    return '-';
}

void SubtractionNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

