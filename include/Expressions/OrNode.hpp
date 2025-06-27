#ifndef OR_NODE_HPP
#define OR_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class OrNode : public BinaryOperatorNode {
public:
    OrNode(ExpressionNode* left, ExpressionNode* right);

    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
