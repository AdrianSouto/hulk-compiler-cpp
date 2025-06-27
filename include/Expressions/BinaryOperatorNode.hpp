#ifndef BINARY_OPERATOR_NODE_HPP
#define BINARY_OPERATOR_NODE_HPP

#include "AST/ExpressionNode.hpp"

class BinaryOperatorNode : public ExpressionNode {
public:
    ExpressionNode* left;
    ExpressionNode* right;

    BinaryOperatorNode(ExpressionNode* left, ExpressionNode* right);

    virtual char getOperator() const = 0;

    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override = 0;

    ~BinaryOperatorNode();
};

#endif
