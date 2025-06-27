#include "Expressions/SelfMemberAccessNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

SelfMemberAccessNode::SelfMemberAccessNode(const std::string& attrName)
    : attributeName(attrName) {
}


bool SelfMemberAccessNode::validate(IContext* context) {
    
    
    return true;
}

void SelfMemberAccessNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
