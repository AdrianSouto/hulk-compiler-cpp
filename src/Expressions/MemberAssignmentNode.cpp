#include "Expressions/MemberAssignmentNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Globals.hpp"
#include <iostream>

MemberAssignmentNode::MemberAssignmentNode(ExpressionNode* obj, const std::string& mem, ExpressionNode* val)
    : object(obj), member(mem), value(val) {}

bool MemberAssignmentNode::validate(IContext* context) {

    if (object && !object->validate(context)) {
        return false;
    }
    

    if (value && !value->validate(context)) {
        return false;
    }
    
    return true;
}

void MemberAssignmentNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

MemberAssignmentNode::~MemberAssignmentNode() {
    delete object;
    delete value;
}