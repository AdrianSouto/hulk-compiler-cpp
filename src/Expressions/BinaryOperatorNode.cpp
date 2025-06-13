#include "Expressions/BinaryOperatorNode.hpp"
#include "Context/IContext.hpp"
#include <iostream>

BinaryOperatorNode::BinaryOperatorNode(ExpressionNode* left, ExpressionNode* right)
    : left(left), right(right) {}

BinaryOperatorNode::~BinaryOperatorNode() {
    delete left;
    delete right;
}

bool BinaryOperatorNode::validate(IContext* context) {
    if (!left->validate(context)) {
        errorMessage = "Error in left operand of '" + std::string(1, getOperator()) + "': " +
                       left->getErrorMessage();
        return false;
    }

    if (!right->validate(context)) {
        errorMessage = "Error in right operand of '" + std::string(1, getOperator()) + "': " +
                       right->getErrorMessage();
        return false;
    }

    return true;
}

void BinaryOperatorNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Operator: " << getOperator() << std::endl;

    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "Left: ";
    if (left) {
        left->print(indent + 1);
    } else {
        std::cout << "nullptr" << std::endl;
    }

    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "Right: ";
    if (right) {
        right->print(indent + 1);
    } else {
        std::cout << "nullptr" << std::endl;
    }
}
