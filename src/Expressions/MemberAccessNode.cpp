#include "Expressions/MemberAccessNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

MemberAccessNode::~MemberAccessNode() {
    delete object;
}




bool MemberAccessNode::validate(IContext* context) {
    
    if (!object->validate(context)) {
        errorMessage = "Error in member access object: " + object->getErrorMessage();
        return false;
    }
    
    
    
    
    return true;
}

void MemberAccessNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}