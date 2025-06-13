#include "Expressions/LessNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

LessNode::LessNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}

int LessNode::evaluate() const {
    return left->evaluate() < right->evaluate() ? 1 : 0;
}

char LessNode::getOperator() const {
    return '<';
}

void LessNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}