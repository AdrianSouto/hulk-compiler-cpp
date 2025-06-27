#include "AST/ExpressionNode.hpp"
#include "Types/Type.hpp"

Type* ExpressionNode::inferType(IContext* context) const {
    (void)context;
    // Default implementation - derived classes should override this
    return Type::getNumberType();
}
