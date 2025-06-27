#ifndef STRING_LITERAL_NODE_HPP
#define STRING_LITERAL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class Type;

class StringLiteralNode : public ExpressionNode {
public:
    std::string value;

    StringLiteralNode(const std::string& val);


    bool validate(IContext* context) override { (void)context; return true; }
    Type* inferType(IContext* context) const override;
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

