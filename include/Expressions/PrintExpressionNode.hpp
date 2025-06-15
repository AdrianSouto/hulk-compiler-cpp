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
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    int evaluate() const override;
    double evaluateDouble() const override;
    std::string evaluateString() const override;
};