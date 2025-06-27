#include "Expressions/LessNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

LessNode::LessNode(ExpressionNode* l, ExpressionNode* r) : BinaryOperatorNode(l, r) {}


char LessNode::getOperator() const {
    return '<';
}

void LessNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}