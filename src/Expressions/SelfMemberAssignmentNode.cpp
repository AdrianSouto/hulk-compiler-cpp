#include "Expressions/SelfMemberAssignmentNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Globals.hpp"
#include <iostream>

SelfMemberAssignmentNode::SelfMemberAssignmentNode(const std::string& mem, ExpressionNode* val)
    : member(mem), value(val) {}

int SelfMemberAssignmentNode::evaluate() const {

    return value ? value->evaluate() : 0;
}

std::string SelfMemberAssignmentNode::evaluateString() const {

    return value ? value->evaluateString() : "";
}

void SelfMemberAssignmentNode::print(int indent) const {
    for (int i = 0; i < indent; i++) std::cout << "  ";
    std::cout << "SelfMemberAssignment: self." << member << " := ";
    if (value) {
        std::cout << std::endl;
        value->print(indent + 1);
    } else {
        std::cout << "null" << std::endl;
    }
}

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