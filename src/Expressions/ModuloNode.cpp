#include "Expressions/ModuloNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

ModuloNode::ModuloNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int ModuloNode::evaluate() const {
    
    return left->evaluate() % right->evaluate();
}

char ModuloNode::getOperator() const {
    return '%';
}

void ModuloNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

