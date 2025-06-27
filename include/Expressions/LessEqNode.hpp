#ifndef LESS_EQ_NODE_HPP
#define LESS_EQ_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class LessEqNode : public BinaryOperatorNode {
public:
    LessEqNode(ExpressionNode* left, ExpressionNode* right);


    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
