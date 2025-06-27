#ifndef IS_NODE_HPP
#define IS_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class IsNode : public ExpressionNode {
public:
    ExpressionNode* expression;
    std::string typeName;
    
    IsNode(ExpressionNode* expr, const std::string& type) 
        : expression(expr), typeName(type) {}

    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    
    ~IsNode();
};

#endif