#ifndef NUMBER_NODE_HPP
#define NUMBER_NODE_HPP

#include "AST/ExpressionNode.hpp"

class NumberNode : public ExpressionNode {
public:
    int value;

    NumberNode(int val);

    int evaluate() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override { (void)context; return true; }
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

