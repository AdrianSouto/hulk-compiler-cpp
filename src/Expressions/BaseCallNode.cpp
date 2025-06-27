#include "Expressions/BaseCallNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include <iostream>

BaseCallNode::BaseCallNode(const std::vector<ExpressionNode*>& arguments)
    : args(arguments) {}



bool BaseCallNode::validate(IContext* context) {
    
    for (size_t i = 0; i < args.size(); i++) {
        if (!args[i]->validate(context)) {
            errorMessage = "Error in argument " + std::to_string(i+1) + " of base() call: " + args[i]->getErrorMessage();
            return false;
        }
    }

    
    
    
    
    
    
    

    
    
    
    
    

    return true;
}

void BaseCallNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

BaseCallNode::~BaseCallNode() {
    for (auto arg : args) {
        delete arg;
    }
}