#ifndef METHOD_CALL_NODE_HPP
#define METHOD_CALL_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>
#include <vector>

class MethodCallNode : public ExpressionNode {
public:
    ExpressionNode* object;
    std::string methodName;
    std::vector<ExpressionNode*> arguments;
    
    MethodCallNode(ExpressionNode* obj, const std::string& method, const std::vector<ExpressionNode*>& args)
        : object(obj), methodName(method), arguments(args) {}

    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    
    ~MethodCallNode();
};

#endif