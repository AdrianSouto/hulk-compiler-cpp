#ifndef VARIABLE_NODE_HPP
#define VARIABLE_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class Type;

class VariableNode : public ExpressionNode {
public:
    std::string identifier;

    VariableNode(const std::string& id);


    bool validate(IContext* context) override;
    Type* inferType(IContext* context) const;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

