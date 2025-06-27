#include "AST/Attribute.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>



bool Attribute::validate(IContext* context) {
    if (hasInitExpression()) {
        return initExpression->validate(context);
    }
    return true;
}

void Attribute::accept(LLVMCodegenVisitor& visitor) {

    (void)visitor; 
}