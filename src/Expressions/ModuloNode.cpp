#include "Expressions/ModuloNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

ModuloNode::ModuloNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}


char ModuloNode::getOperator() const {
    return '%';
}

void ModuloNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

