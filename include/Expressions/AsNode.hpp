#ifndef AS_NODE_HPP
#define AS_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class AsNode : public ExpressionNode {
public:
    ExpressionNode* expression;
    std::string typeName;
    
    AsNode(ExpressionNode* expr, const std::string& type) 
        : expression(expr), typeName(type) {}
    
    int evaluate() const override;
    std::string evaluateString() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    
    ~AsNode();
};

#endif