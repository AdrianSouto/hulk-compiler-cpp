#include "Expressions/GreaterNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

GreaterNode::GreaterNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}



char GreaterNode::getOperator() const {
    return '>';
}

void GreaterNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}