#ifndef EXPRESSION_STATEMENT_NODE_HPP
#define EXPRESSION_STATEMENT_NODE_HPP

#include "AST/StatementNode.hpp"
#include "AST/ExpressionNode.hpp"

class ExpressionStatementNode : public StatementNode {
public:
    ExpressionNode* expression;

    ExpressionStatementNode(ExpressionNode* expr);

    void execute() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;

    ~ExpressionStatementNode();
};

#endif 