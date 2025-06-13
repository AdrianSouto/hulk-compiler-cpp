#ifndef MEMBER_ACCESS_NODE_HPP
#define MEMBER_ACCESS_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class MemberAccessNode : public ExpressionNode {
public:
    ExpressionNode* object;
    std::string memberName;
    
    MemberAccessNode(ExpressionNode* obj, const std::string& member)
        : object(obj), memberName(member) {}
    
    int evaluate() const override;
    void execute() const;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    
    ~MemberAccessNode();
};

#endif