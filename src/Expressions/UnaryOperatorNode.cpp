#include "Expressions/UnaryOperatorNode.hpp"
#include "Context/IContext.hpp"
#include <iostream>

UnaryOperatorNode::UnaryOperatorNode(ExpressionNode* operand)
    : operand(operand) {}

UnaryOperatorNode::~UnaryOperatorNode() {
    delete operand;
}

bool UnaryOperatorNode::validate(IContext* context) {
    if (!operand->validate(context)) {
        errorMessage = "Error in operand of unary '" + std::string(1, getOperator()) + "': " +
                       operand->getErrorMessage();
        return false;
    }
    return true;
}


