#ifndef NOT_NODE_HPP
#define NOT_NODE_HPP

#include "Expressions/UnaryOperatorNode.hpp"

class NotNode : public UnaryOperatorNode {
public:
    NotNode(ExpressionNode* operand);


    char getOperator() const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
