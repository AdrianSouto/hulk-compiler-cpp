#ifndef EXPRESSION_NODE_HPP
#define EXPRESSION_NODE_HPP

#include "AST/ASTNode.hpp"
#include <string>

class ExpressionNode : public ASTNode {
public:
    virtual int evaluate() const = 0;
    virtual std::string evaluateString() const;
    void accept(LLVMCodegenVisitor& visitor) override = 0;
    ~ExpressionNode() override = default;
};

#endif
