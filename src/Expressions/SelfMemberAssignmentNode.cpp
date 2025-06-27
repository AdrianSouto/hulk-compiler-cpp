#include "Expressions/SelfMemberAssignmentNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Globals.hpp"
#include <iostream>

SelfMemberAssignmentNode::SelfMemberAssignmentNode(const std::string& mem, ExpressionNode* val)
    : member(mem), value(val) {}

bool SelfMemberAssignmentNode::validate(IContext* context) {

    if (value && !value->validate(context)) {
        return false;
    }
    
    return true;
}

void SelfMemberAssignmentNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

SelfMemberAssignmentNode::~SelfMemberAssignmentNode() {
    delete value;
}