#ifndef MEMBER_ASSIGNMENT_NODE_HPP
#define MEMBER_ASSIGNMENT_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class MemberAssignmentNode : public ExpressionNode {
public:
    ExpressionNode* object;
    std::string member;
    ExpressionNode* value;

    MemberAssignmentNode(ExpressionNode* obj, const std::string& mem, ExpressionNode* val);


    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;

    ~MemberAssignmentNode();
};

#endif