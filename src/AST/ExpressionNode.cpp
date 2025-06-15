#include "AST/ExpressionNode.hpp"
#include "Types/Type.hpp"

std::string ExpressionNode::evaluateString() const {
    return std::to_string(evaluate());
}

Type* ExpressionNode::inferType(IContext* context) const {
    (void)context;

    return Type::getNumberType();
}
