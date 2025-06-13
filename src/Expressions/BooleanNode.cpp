#include "Expressions/BooleanNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

BooleanNode::BooleanNode(bool val) : value(val) {}

int BooleanNode::evaluate() const {
    return value ? 1 : 0;  
}

std::string BooleanNode::evaluateString() const {
    return value ? "true" : "false";
}

void BooleanNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Boolean: " << (value ? "true" : "false") << std::endl;
}

bool BooleanNode::validate(IContext* context) { 
    return true; 
}

void BooleanNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
