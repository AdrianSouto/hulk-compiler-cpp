#ifndef UNARY_OPERATOR_NODE_HPP
#define UNARY_OPERATOR_NODE_HPP

#include "AST/ExpressionNode.hpp"

class UnaryOperatorNode : public ExpressionNode {
public:
    ExpressionNode* operand;

    UnaryOperatorNode(ExpressionNode* operand);

    virtual char getOperator() const = 0;

    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override = 0;

    ~UnaryOperatorNode();
};

#endif
