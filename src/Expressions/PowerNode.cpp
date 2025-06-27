#include "Expressions/PowerNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <cmath>

PowerNode::PowerNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}



char PowerNode::getOperator() const {
    return '^';
}

void PowerNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
