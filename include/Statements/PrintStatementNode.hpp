#ifndef PRINT_STATEMENT_NODE_HPP
#define PRINT_STATEMENT_NODE_HPP

#include "AST/StatementNode.hpp"
#include "AST/ExpressionNode.hpp"

class PrintStatementNode : public StatementNode {
public:
    ExpressionNode* expression;

    PrintStatementNode(ExpressionNode* expr);

    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;

    ~PrintStatementNode();
};

#endif 

