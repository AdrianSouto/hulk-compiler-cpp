#include "Expressions/PowerNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <cmath>

PowerNode::PowerNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int PowerNode::evaluate() const {
    return static_cast<int>(std::pow(left->evaluate(), right->evaluate()));
}

char PowerNode::getOperator() const {
    return '^';
}

void PowerNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
