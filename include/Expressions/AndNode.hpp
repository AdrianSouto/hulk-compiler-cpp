#ifndef AND_NODE_HPP
#define AND_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class AndNode : public BinaryOperatorNode {
public:
    AndNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
