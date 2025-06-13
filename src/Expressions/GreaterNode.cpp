#include "Expressions/GreaterNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

GreaterNode::GreaterNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}

int GreaterNode::evaluate() const {
    return left->evaluate() > right->evaluate() ? 1 : 0;
}

char GreaterNode::getOperator() const {
    return '>';
}

void GreaterNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}