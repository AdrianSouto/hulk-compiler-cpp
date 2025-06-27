#include "Expressions/StringLiteralNode.hpp"
#include "Types/Type.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

StringLiteralNode::StringLiteralNode(const std::string& val) : value(val) {}


Type* StringLiteralNode::inferType(IContext* context) const {
    (void)context;
    return Type::getStringType();
}

void StringLiteralNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

