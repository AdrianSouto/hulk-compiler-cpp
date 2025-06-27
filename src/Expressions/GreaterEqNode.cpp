#include "Expressions/GreaterEqNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

GreaterEqNode::GreaterEqNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}



char GreaterEqNode::getOperator() const {
    return 'G'; 
}

void GreaterEqNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}