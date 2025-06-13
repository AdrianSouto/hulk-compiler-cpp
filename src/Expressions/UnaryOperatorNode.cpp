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

void UnaryOperatorNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Unary Operator: " << getOperator() << std::endl;

    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "Operand: ";
    if (operand) {
        operand->print(indent + 1);
    } else {
        std::cout << "nullptr" << std::endl;
    }
}
