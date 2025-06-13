#ifndef EQUAL_NODE_HPP
#define EQUAL_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class EqualNode : public BinaryOperatorNode {
public:
    EqualNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
