#ifndef STATEMENT_NODE_HPP
#define STATEMENT_NODE_HPP

#include "AST/ASTNode.hpp"

class StatementNode : public ASTNode {
public:
    virtual void execute() const = 0;
    void accept(LLVMCodegenVisitor& visitor) override = 0;
    ~StatementNode() override = default;
};

#endif
