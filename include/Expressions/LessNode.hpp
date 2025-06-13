#ifndef LESS_NODE_HPP
#define LESS_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class LessNode : public BinaryOperatorNode {
public:
    LessNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
