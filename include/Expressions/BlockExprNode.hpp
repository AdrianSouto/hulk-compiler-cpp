#ifndef BLOCK_EXPR_NODE_HPP
#define BLOCK_EXPR_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <vector>

class BlockExprNode : public ExpressionNode {
public:
    std::vector<ExpressionNode*> expressions;

    BlockExprNode(const std::vector<ExpressionNode*>& exprs);
    virtual ~BlockExprNode();

    virtual bool validate(IContext* context) override;
    virtual void accept(LLVMCodegenVisitor& visitor) override;
};

#endif