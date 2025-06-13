#ifndef CONDITIONAL_NODE_HPP
#define CONDITIONAL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include "AST/ASTNode.hpp"
#include <vector>

class ConditionalNode : public ExpressionNode {
public:
    struct Branch {
        ExpressionNode* condition;
        ASTNode* body;
        Branch(ExpressionNode* cond, ASTNode* b) : condition(cond), body(b) {}
    };

    std::vector<Branch> branches;
    ASTNode* elseBody;

    ConditionalNode();
    void addBranch(ExpressionNode* condition, ASTNode* body);
    void setElse(ASTNode* elseBody);
    void execute() const {}
    int evaluate() const override {
        return 0;
    }
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~ConditionalNode();
};

#endif

