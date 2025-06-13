#include "Expressions/StringLiteralNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

StringLiteralNode::StringLiteralNode(const std::string& val) : value(val) {}

int StringLiteralNode::evaluate() const {
    try {
        return std::stoi(value);
    } catch(...) {
        return 0;
    }
}

std::string StringLiteralNode::evaluateString() const {
    return value;
}

void StringLiteralNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "String: \"" << value << "\"" << std::endl;
}

void StringLiteralNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

