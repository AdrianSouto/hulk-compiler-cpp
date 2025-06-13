#include "Expressions/MemberAssignmentNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Globals.hpp"
#include <iostream>

MemberAssignmentNode::MemberAssignmentNode(ExpressionNode* obj, const std::string& mem, ExpressionNode* val)
    : object(obj), member(mem), value(val) {}

int MemberAssignmentNode::evaluate() const {

    return value ? value->evaluate() : 0;
}

std::string MemberAssignmentNode::evaluateString() const {

    return value ? value->evaluateString() : "";
}

void MemberAssignmentNode::print(int indent) const {
    for (int i = 0; i < indent; i++) std::cout << "  ";
    std::cout << "MemberAssignment: ";
    if (object) {
        object->print(0);
    }
    std::cout << "." << member << " := ";
    if (value) {
        std::cout << std::endl;
        value->print(indent + 1);
    } else {
        std::cout << "null" << std::endl;
    }
}

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