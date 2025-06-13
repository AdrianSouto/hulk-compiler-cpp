#ifndef STRING_LITERAL_NODE_HPP
#define STRING_LITERAL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class StringLiteralNode : public ExpressionNode {
public:
    std::string value;

    StringLiteralNode(const std::string& val);

    int evaluate() const override;
    std::string evaluateString() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override { (void)context; return true; }
    void accept(LLVMCodegenVisitor& visitor) override;
};

#endif

