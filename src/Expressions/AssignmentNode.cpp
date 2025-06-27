#include "Expressions/AssignmentNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Globals.hpp"
#include <iostream>

AssignmentNode::AssignmentNode(const std::string& id, ExpressionNode* val)
    : identifier(id), value(val) {}



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