#ifndef ASSIGNMENT_NODE_HPP
#define ASSIGNMENT_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class AssignmentNode : public ExpressionNode {
public:
    std::string identifier;
    ExpressionNode* value;

    AssignmentNode(const std::string& id, ExpressionNode* val);


    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;

    ~AssignmentNode();
};

#endif