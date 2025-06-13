#include "Expressions/AssignmentNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Globals.hpp"
#include <iostream>

AssignmentNode::AssignmentNode(const std::string& id, ExpressionNode* val)
    : identifier(id), value(val) {}

int AssignmentNode::evaluate() const {
    
    int assignedValue = value->evaluate();
    
    
    variables[identifier] = std::to_string(assignedValue);
    
    
    return assignedValue;
}

std::string AssignmentNode::evaluateString() const {
    
    std::string assignedValue = value->evaluateString();
    
    
    variables[identifier] = assignedValue;
    
    
    return assignedValue;
}

void AssignmentNode::print(int indent) const {
    for (int i = 0; i < indent; i++) std::cout << "  ";
    std::cout << "Assignment: " << identifier << " := ";
    if (value) {
        std::cout << std::endl;
        value->print(indent + 1);
    } else {
        std::cout << "null" << std::endl;
    }
}

bool AssignmentNode::validate(IContext* context) {
    
    if (identifier == "self") {
        std::cerr << "Semantic error: 'self' is not a valid assignment target" << std::endl;
        return false;
    }
    
    
    if (!context->IsDefined(identifier)) {
        std::cerr << "Error: Variable '" << identifier << "' not declared" << std::endl;
        return false;
    }
    
    
    if (value && !value->validate(context)) {
        return false;
    }
    
    return true;
}

void AssignmentNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

AssignmentNode::~AssignmentNode() {
    delete value;
}