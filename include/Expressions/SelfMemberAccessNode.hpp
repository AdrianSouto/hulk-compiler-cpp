#ifndef SELF_MEMBER_ACCESS_NODE_HPP
#define SELF_MEMBER_ACCESS_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>

class SelfMemberAccessNode : public ExpressionNode {
public:
    std::string attributeName;

    SelfMemberAccessNode(const std::string& attrName);


    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~SelfMemberAccessNode() = default;
};

#endif
