#ifndef BLOCK_NODE_HPP
#define BLOCK_NODE_HPP

#include "../AST/StatementNode.hpp"
#include <vector>

class BlockNode : public StatementNode {
public:
    std::vector<StatementNode*> statements;

    BlockNode(const std::vector<StatementNode*>& stmts);
    virtual ~BlockNode();

    virtual void print(int indent = 0) const override;
    virtual bool validate(IContext* context) override;
    virtual void execute() const override;
    virtual void accept(LLVMCodegenVisitor& visitor) override;
};

#endif 
