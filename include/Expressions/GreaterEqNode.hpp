#ifndef GREATER_EQ_NODE_HPP
#define GREATER_EQ_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class GreaterEqNode : public BinaryOperatorNode {
public:
    GreaterEqNode(ExpressionNode* left, ExpressionNode* right);

    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
