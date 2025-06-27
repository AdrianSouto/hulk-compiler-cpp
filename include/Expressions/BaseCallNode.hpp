#ifndef BASE_CALL_NODE_HPP
#define BASE_CALL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <vector>

class BaseCallNode : public ExpressionNode {
public:
    std::vector<ExpressionNode*> args;

    BaseCallNode(const std::vector<ExpressionNode*>& arguments);

    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~BaseCallNode();
};

#endif