#include "Expressions/NumberNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>
#include <cmath>

NumberNode::NumberNode(int val) : value(static_cast<double>(val)), isInteger(true) {}

NumberNode::NumberNode(double val) : value(val), isInteger(false) {}

int NumberNode::evaluate() const {
    return static_cast<int>(value);
}

double NumberNode::evaluateDouble() const {
    return value;
}

void NumberNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    if (isInteger) {
        std::cout << "Number: " << static_cast<int>(value) << std::endl;
    } else {
        std::cout << "Number: " << value << std::endl;
    }
}

void NumberNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

