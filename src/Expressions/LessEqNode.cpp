#include "Expressions/LessEqNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

LessEqNode::LessEqNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}

int LessEqNode::evaluate() const {
    return left->evaluate() <= right->evaluate() ? 1 : 0;
}

char LessEqNode::getOperator() const {
    return 'L'; 
}

void LessEqNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}