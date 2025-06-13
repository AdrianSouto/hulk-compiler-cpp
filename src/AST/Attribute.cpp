#include "AST/Attribute.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

void Attribute::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Attribute: " << toString() << std::endl;
}

bool Attribute::validate(IContext* context) {
    if (hasInitExpression()) {
        return initExpression->validate(context);
    }
    return true;
}

void Attribute::accept(LLVMCodegenVisitor& visitor) {

    (void)visitor; 
}