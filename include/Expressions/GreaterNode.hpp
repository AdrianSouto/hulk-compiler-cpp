#ifndef GREATER_NODE_HPP
#define GREATER_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class GreaterNode : public BinaryOperatorNode {
public:
    GreaterNode(ExpressionNode* left, ExpressionNode* right);

    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
