#ifndef LET_EXPR_NODE_HPP
#define LET_EXPR_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include "AST/VarDeclPair.hpp"
#include <vector>
#include <string>

class LetExprNode : public ExpressionNode {
private:
    std::vector<VarDeclPair> declarations;
    ExpressionNode* body;

public:
    LetExprNode(const std::vector<VarDeclPair>& decls, ExpressionNode* expr);
    ~LetExprNode() override;
    
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    const std::vector<VarDeclPair>& getDeclarations() const { return declarations; }
    ExpressionNode* getBody() const { return body; }
};

#endif
