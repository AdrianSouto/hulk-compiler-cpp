#ifndef EXPRESSION_NODE_HPP
#define EXPRESSION_NODE_HPP

#include "AST/ASTNode.hpp"
#include <string>

class Type;

class ExpressionNode : public ASTNode {
public:
    virtual Type* inferType(IContext* context) const;
    void accept(LLVMCodegenVisitor& visitor) override = 0;
    ~ExpressionNode() override = default;
};

#endif
