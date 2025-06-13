#include "Expressions/NumberNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

NumberNode::NumberNode(int val) : value(val) {}

int NumberNode::evaluate() const {
    return value;
}

void NumberNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Number: " << evaluate() << std::endl;
}

void NumberNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

