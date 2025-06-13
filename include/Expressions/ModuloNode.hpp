#ifndef MODULO_NODE_HPP
#define MODULO_NODE_HPP

#include "Expressions/BinaryOperatorNode.hpp"

class ModuloNode : public BinaryOperatorNode {
public:
    ModuloNode(ExpressionNode* left, ExpressionNode* right);

    int evaluate() const override;
    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

