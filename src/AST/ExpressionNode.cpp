#include "AST/ExpressionNode.hpp"

std::string ExpressionNode::evaluateString() const {
    return std::to_string(evaluate());
}
