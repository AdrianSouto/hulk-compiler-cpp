#include "Expressions/NotNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"

NotNode::NotNode(ExpressionNode* operand)
    : UnaryOperatorNode(operand) {}



char NotNode::getOperator() const {
    return '!';
}

void NotNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
