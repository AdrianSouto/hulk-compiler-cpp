#ifndef UNARY_MINUS_NODE_HPP
#define UNARY_MINUS_NODE_HPP

#include "Expressions/UnaryOperatorNode.hpp"

class UnaryMinusNode : public UnaryOperatorNode {
public:
    UnaryMinusNode(ExpressionNode* operand);

    int evaluate() const override;
    double evaluateDouble() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif