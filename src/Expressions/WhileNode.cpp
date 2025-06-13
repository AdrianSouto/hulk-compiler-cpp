#include "Expressions/WhileNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include <iostream>

WhileNode::WhileNode(ExpressionNode* cond, ASTNode* b) 
    : condition(cond), body(b) {}

int WhileNode::evaluate() const {
    
    
    return 0;
}

void WhileNode::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "While:" << std::endl;
    std::cout << indentStr << "  Condition:" << std::endl;
    if (condition) {
        condition->print(indent + 4);
    }
    std::cout << indentStr << "  Body:" << std::endl;
    if (body) {
        body->print(indent + 4);
    }
}

bool WhileNode::validate(IContext* context) {
    if (!condition || !condition->validate(context)) {
        return false;
    }
    
    
    IContext* childContext = context->CreateChildContext();
    bool valid = body && body->validate(childContext);
    delete childContext;
    
    return valid;
}

void WhileNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

WhileNode::~WhileNode() {
    delete condition;
    delete body;
}