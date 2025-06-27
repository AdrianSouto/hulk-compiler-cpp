#include "Expressions/NumberNode.hpp"
#include "Types/Type.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>
#include <cmath>

NumberNode::NumberNode(int val) : value(static_cast<double>(val)), isInteger(true) {}

NumberNode::NumberNode(double val) : value(val), isInteger(false) {}


Type* NumberNode::inferType(IContext* context) const {
    (void)context;
    return Type::getNumberType();
}

void NumberNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

