#include "Expressions/BaseCallNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include <iostream>

BaseCallNode::BaseCallNode(const std::vector<ExpressionNode*>& arguments)
    : args(arguments) {}

int BaseCallNode::evaluate() const {
    
    
    return 0;
}

void BaseCallNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "BaseCall()" << std::endl;
    for (size_t i = 0; i < args.size(); ++i) {
        for (int j = 0; j < indent + 1; ++j) {
            std::cout << "  ";
        }
        std::cout << "Arg " << i << ": ";
        args[i]->print(indent + 2);
    }
}

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