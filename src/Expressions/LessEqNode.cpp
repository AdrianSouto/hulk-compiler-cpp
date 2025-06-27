#include "Expressions/LessEqNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

LessEqNode::LessEqNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}



char LessEqNode::getOperator() const {
    return 'L'; 
}

void LessEqNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}