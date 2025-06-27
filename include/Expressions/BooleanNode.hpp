#ifndef BOOLEAN_NODE_HPP
#define BOOLEAN_NODE_HPP

#include "AST/ExpressionNode.hpp"

class BooleanNode : public ExpressionNode {
public:
    bool value;

    BooleanNode(bool val);

    bool validate(IContext* context) override;
    Type* inferType(IContext* context) const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif
