#ifndef ADDITION_NODE_HPP
#define ADDITION_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class AdditionNode : public BinaryOperatorNode {
public:
    AdditionNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

