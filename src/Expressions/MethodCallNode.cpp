#include "Expressions/MethodCallNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Globals.hpp"
#include <iostream>

MethodCallNode::~MethodCallNode() {
    delete object;
    for (auto arg : arguments) {
        delete arg;
    }
}


bool MethodCallNode::validate(IContext* context) {
    
    if (!object->validate(context)) {
        errorMessage = "Error in method call object: " + object->getErrorMessage();
        return false;
    }
    
    
    for (size_t i = 0; i < arguments.size(); i++) {
        if (!arguments[i]->validate(context)) {
            errorMessage = "Error in argument " + std::to_string(i+1) + " of method '" +
                           methodName + "': " + arguments[i]->getErrorMessage();
            return false;
        }
    }
    
    
    
    
    return true;
}

void MethodCallNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}