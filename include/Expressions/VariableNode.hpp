#ifndef VARIABLE_NODE_HPP
#define VARIABLE_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class VariableNode : public ExpressionNode {
public:
    std::string identifier;

    VariableNode(const std::string& id);

    int evaluate() const override;
    std::string evaluateString() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

