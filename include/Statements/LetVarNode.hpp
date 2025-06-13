#ifndef LET_VAR_NODE_HPP
#define LET_VAR_NODE_HPP

#include "AST/StatementNode.hpp"
#include "AST/ExpressionNode.hpp"
#include "Types/Type.hpp"
#include "AST/ASTNode.hpp"
#include <string>

class LetVarNode : public StatementNode {
public:
    std::string identifier;
    ExpressionNode* expr;
    ASTNode* body;
    Type* declaredType;  

    LetVarNode(const std::string& id, ExpressionNode* expr, ASTNode* body, Type* type = Type::getUnknownType());

    void execute() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~LetVarNode();
};

#endif 
