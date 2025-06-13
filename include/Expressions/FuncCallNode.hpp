#ifndef FUNC_CALL_NODE_HPP
#define FUNC_CALL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>
#include <vector>

class FuncCallNode : public ExpressionNode {
public:
    std::string identifier;
    std::vector<ExpressionNode*> args;

    FuncCallNode(const std::string& id, const std::vector<ExpressionNode*>& arguments);

    int evaluate() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~FuncCallNode();
};

#endif
