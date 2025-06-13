#ifndef POWER_NODE_HPP
#define POWER_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class PowerNode : public BinaryOperatorNode {
public:
    PowerNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
