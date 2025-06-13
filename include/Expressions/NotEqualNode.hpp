#ifndef NOT_EQUAL_NODE_HPP
#define NOT_EQUAL_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class NotEqualNode : public BinaryOperatorNode {
public:
    NotEqualNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
