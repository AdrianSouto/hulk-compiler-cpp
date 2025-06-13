#include "Expressions/VariableNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include <iostream>

VariableNode::VariableNode(const std::string& id) : identifier(id) {}

int VariableNode::evaluate() const {
    
    if (identifier == "PI") {
        return 3; 
    }
    
    if (variables.find(identifier) != variables.end()) {
        try {
            return std::stoi(variables[identifier]);
        } catch(...) {
            return 0;
        }
    }
    std::cerr << "Error: Variable '" << identifier << "' not defined." << std::endl;
    return 0;
}

std::string VariableNode::evaluateString() const {
    
    if (identifier == "PI") {
        return "3.141592653589793"; 
    }
    
    if (variables.find(identifier) != variables.end()) {
        return variables[identifier];
    }
    std::cerr << "Error: Variable '" << identifier << "' not defined." << std::endl;
    return "";
}

void VariableNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Variable: " << identifier << std::endl;
}

bool VariableNode::validate(IContext* context) {
    if (!context->IsDefined(identifier)) {
        errorMessage = "Error: Variable '" + identifier + "' is not defined in the current scope";
        return false;
    }
    return true;
}

void VariableNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

