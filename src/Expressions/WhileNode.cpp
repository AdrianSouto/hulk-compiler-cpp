#include "Expressions/WhileNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include <iostream>

WhileNode::WhileNode(ExpressionNode* cond, ASTNode* b) 
    : condition(cond), body(b) {}



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