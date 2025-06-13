#ifndef VAR_DECL_PAIR_HPP
#define VAR_DECL_PAIR_HPP

#include <string>
#include "../AST/ExpressionNode.hpp"
#include "Types/Type.hpp"
class ExpressionNode;
struct VarDeclPair {
    std::string id;
    ExpressionNode* expr;
    Type* type;

    VarDeclPair() : expr(nullptr), type(Type::getUnknownType()) {}
};

#endif
