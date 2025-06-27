#ifndef DIVISION_NODE_HPP
#define DIVISION_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class DivisionNode : public BinaryOperatorNode {
public:
    DivisionNode(ExpressionNode* left, ExpressionNode* right);

    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

