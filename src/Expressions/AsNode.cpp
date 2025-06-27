#include "Expressions/AsNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Types/Type.hpp"
#include "Globals.hpp"
#include <iostream>
#include <stdexcept>

AsNode::~AsNode() {
    delete expression;
}



bool AsNode::validate(IContext* context) {
    
    if (!expression->validate(context)) {
        errorMessage = "Error in 'as' expression: " + expression->getErrorMessage();
        return false;
    }
    
    
    
    
    
    return true;
}

Type* AsNode::inferType(IContext* context) const {
    // The result type of an 'as' expression is the target type
    Type* targetType = getTypeByName(typeName);
    if (targetType) {
        return targetType;
    }
    
    // If target type is not found, return the default
    return Type::getNumberType();
}

void AsNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}