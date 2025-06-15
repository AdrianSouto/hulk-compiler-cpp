#ifndef EXPRESSION_NODE_HPP
#define EXPRESSION_NODE_HPP

#include "AST/ASTNode.hpp"
#include <string>

class Type;

class ExpressionNode : public ASTNode {
public:
    virtual int evaluate() const = 0;
    virtual double evaluateDouble() const { return static_cast<double>(evaluate()); }
    virtual std::string evaluateString() const;
    virtual Type* inferType(IContext* context) const;
    void accept(LLVMCodegenVisitor& visitor) override = 0;
    ~ExpressionNode() override = default;
};

#endif
