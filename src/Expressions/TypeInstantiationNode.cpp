#include "Expressions/TypeInstantiationNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

TypeInstantiationNode::~TypeInstantiationNode() {
    for (auto arg : arguments) {
        delete arg;
    }
}

int TypeInstantiationNode::evaluate() const {
    
    
    return 0;
}

void TypeInstantiationNode::execute() const {
    std::cout << "Creating instance of type '" << typeName << "' with " 
              << arguments.size() << " arguments" << std::endl;
}

void TypeInstantiationNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "TypeInstantiation: " << typeName << "(";
    
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) std::cout << ", ";
        arguments[i]->print(0);
    }
    std::cout << ")" << std::endl;
}

bool TypeInstantiationNode::validate(IContext* context) {
    
    
    
    
    
    for (auto arg : arguments) {
        if (!arg->validate(context)) {
            errorMessage = "Error in type instantiation argument: " + arg->getErrorMessage();
            return false;
        }
    }
    
    return true;
}

void TypeInstantiationNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}