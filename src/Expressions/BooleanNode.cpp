#include "Expressions/BooleanNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Types/Type.hpp"
#include <iostream>

BooleanNode::BooleanNode(bool val) : value(val) {}



bool BooleanNode::validate(IContext* context) { 
    return true; 
}

Type* BooleanNode::inferType(IContext* context) const {
    (void)context;
    return Type::getBooleanType();
}

void BooleanNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
