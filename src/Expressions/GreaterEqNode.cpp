#include "Expressions/GreaterEqNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

GreaterEqNode::GreaterEqNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}

int GreaterEqNode::evaluate() const {
    return left->evaluate() >= right->evaluate() ? 1 : 0;
}

char GreaterEqNode::getOperator() const {
    return 'G'; 
}

void GreaterEqNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}