#include "Expressions/SelfMemberAccessNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

SelfMemberAccessNode::SelfMemberAccessNode(const std::string& attrName)
    : attributeName(attrName) {
}

int SelfMemberAccessNode::evaluate() const {
    
    
    std::cerr << "Error: Cannot directly evaluate self." << attributeName << " outside of LLVM context" << std::endl;
    return 0;
}

std::string SelfMemberAccessNode::evaluateString() const {
    return "SelfMember(" + attributeName + ")";
}

void SelfMemberAccessNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "SelfMember: self." << attributeName << std::endl;
}

bool SelfMemberAccessNode::validate(IContext* context) {
    
    
    return true;
}

void SelfMemberAccessNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
