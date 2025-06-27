#ifndef SUBTRACTION_NODE_HPP
#define SUBTRACTION_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class SubtractionNode : public BinaryOperatorNode {
public:
    SubtractionNode(ExpressionNode* left, ExpressionNode* right);


    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
