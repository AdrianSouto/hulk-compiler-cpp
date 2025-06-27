#ifndef CONDITIONAL_NODE_HPP
#define CONDITIONAL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include "AST/ASTNode.hpp"
#include <vector>

class Type;

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

    bool validate(IContext* context) override;
    Type* inferType(IContext* context) const override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~ConditionalNode();

private:
    Type* findCommonBaseType(Type* type1, Type* type2) const;
};

#endif

