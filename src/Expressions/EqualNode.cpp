#include "Expressions/EqualNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

EqualNode::EqualNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int EqualNode::evaluate() const {
    return left->evaluate() == right->evaluate();
}

char EqualNode::getOperator() const {
    return '=';
}

void EqualNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
