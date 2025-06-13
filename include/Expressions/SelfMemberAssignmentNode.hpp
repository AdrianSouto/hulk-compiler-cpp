#ifndef SELF_MEMBER_ASSIGNMENT_NODE_HPP
#define SELF_MEMBER_ASSIGNMENT_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class SelfMemberAssignmentNode : public ExpressionNode {
public:
    std::string member;
    ExpressionNode* value;

    SelfMemberAssignmentNode(const std::string& mem, ExpressionNode* val);

    int evaluate() const override;
    std::string evaluateString() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;

    ~SelfMemberAssignmentNode();
};

#endif