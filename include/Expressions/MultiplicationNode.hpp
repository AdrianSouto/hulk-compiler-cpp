#ifndef MULTIPLICATION_NODE_HPP
#define MULTIPLICATION_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class MultiplicationNode : public BinaryOperatorNode {
public:
    MultiplicationNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

