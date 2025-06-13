#ifndef BOOLEAN_NODE_HPP
#define BOOLEAN_NODE_HPP

#include "AST/ExpressionNode.hpp"

class BooleanNode : public ExpressionNode {
public:
    bool value;

    BooleanNode(bool val);

    int evaluate() const override;
    std::string evaluateString() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
