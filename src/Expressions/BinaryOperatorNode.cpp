#include "Expressions/BinaryOperatorNode.hpp"
#include "Context/IContext.hpp"
#include "Types/Type.hpp"
#include <iostream>

BinaryOperatorNode::BinaryOperatorNode(ExpressionNode* left, ExpressionNode* right)
    : left(left), right(right) {}

BinaryOperatorNode::~BinaryOperatorNode() {
    delete left;
    delete right;
}

bool BinaryOperatorNode::validate(IContext* context) {
    bool leftValid = left->validate(context);
    bool rightValid = right->validate(context);
    
    if (!leftValid && !rightValid) {
        // Both operands have errors
        errorMessage = "Multiple errors in '" + std::string(1, getOperator()) + "' expression:\n" +
                       "  - Left operand: " + left->getErrorMessage() + "\n" +
                       "  - Right operand: " + right->getErrorMessage();
        return false;
    } else if (!leftValid) {
        errorMessage = "Error in left operand of '" + std::string(1, getOperator()) + "': " +
                       left->getErrorMessage();
        return false;
    } else if (!rightValid) {
        errorMessage = "Error in right operand of '" + std::string(1, getOperator()) + "': " +
                       right->getErrorMessage();
        return false;
    }

    // Type checking for arithmetic operators
    char op = getOperator();
    if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%' || op == '^') {
        // Get types of both operands
        Type* leftType = left->inferType(context);
        Type* rightType = right->inferType(context);
        
        // Check if both operands are numbers
        if (!leftType->isNumber() && !rightType->isNumber()) {
            errorMessage = "Type error: operator '" + std::string(1, op) + 
                          "' requires numeric operands, but got " + leftType->toString() + 
                          " and " + rightType->toString();
            return false;
        } else if (!leftType->isNumber()) {
            errorMessage = "Type error: left operand of '" + std::string(1, op) + 
                          "' must be numeric, but got " + leftType->toString();
            return false;
        } else if (!rightType->isNumber()) {
            errorMessage = "Type error: right operand of '" + std::string(1, op) + 
                          "' must be numeric, but got " + rightType->toString();
            return false;
        }
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
