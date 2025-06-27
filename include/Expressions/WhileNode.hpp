#ifndef WHILE_NODE_HPP
#define WHILE_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include "AST/ASTNode.hpp"

class WhileNode : public ExpressionNode {
public:
    ExpressionNode* condition;
    ASTNode* body;

    WhileNode(ExpressionNode* cond, ASTNode* b);
    
;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~WhileNode();
};

#endif