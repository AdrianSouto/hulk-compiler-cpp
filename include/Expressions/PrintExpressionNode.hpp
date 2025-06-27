#pragma once
#include "../AST/ExpressionNode.hpp"
#include "../Types/Type.hpp"
#include "../Context/IContext.hpp"

class LLVMCodegenVisitor;

class PrintExpressionNode : public ExpressionNode {
public:
    ExpressionNode* expression;
    
    PrintExpressionNode(ExpressionNode* expr);
    ~PrintExpressionNode();
    
    void accept(LLVMCodegenVisitor& visitor) override;
    bool validate(IContext* context) override;

};