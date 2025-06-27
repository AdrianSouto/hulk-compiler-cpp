#ifndef FUNC_CALL_NODE_HPP
#define FUNC_CALL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>
#include <vector>

class Type;

class FuncCallNode : public ExpressionNode {
public:
    std::string identifier;
    std::vector<ExpressionNode*> args;

    FuncCallNode(const std::string& id, const std::vector<ExpressionNode*>& arguments);


    bool validate(IContext* context) override;
    Type* inferType(IContext* context) const override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~FuncCallNode();
};

#endif
