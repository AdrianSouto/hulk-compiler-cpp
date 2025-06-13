#include "Expressions/MemberAccessNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

MemberAccessNode::~MemberAccessNode() {
    delete object;
}

int MemberAccessNode::evaluate() const {
    
    
    return 0;
}

void MemberAccessNode::execute() const {
    std::cout << "Accessing member '" << memberName << "' of object" << std::endl;
}

void MemberAccessNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "MemberAccess: ";
    object->print(0);
    std::cout << "." << memberName << std::endl;
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