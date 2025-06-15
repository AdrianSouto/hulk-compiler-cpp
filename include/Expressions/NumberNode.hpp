#ifndef NUMBER_NODE_HPP
#define NUMBER_NODE_HPP

#include "AST/ExpressionNode.hpp"

class NumberNode : public ExpressionNode {
public:
    double value;
    bool isInteger;

    NumberNode(int val);
    NumberNode(double val);

    int evaluate() const override;
    double evaluateDouble() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override { (void)context; return true; }
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

