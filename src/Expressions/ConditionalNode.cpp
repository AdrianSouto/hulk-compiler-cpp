#include "Expressions/ConditionalNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"

#include <iostream>
#include <AST/ASTNode.hpp> 

ConditionalNode::ConditionalNode() : elseBody(nullptr) {}

void ConditionalNode::addBranch(ExpressionNode* condition, ASTNode* body) { 
    branches.emplace_back(condition, body);
}

void ConditionalNode::setElse(ASTNode* elseBody_) { 
    elseBody = elseBody_;
}



void ConditionalNode::print(int indent) const {
    for (const auto& branch : branches) {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "If/Elif Condition:" << std::endl;
        branch.condition->print(indent + 1);
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "Body (ASTNode):" << std::endl; 
        branch.body->print(indent + 1);
    }
    if (elseBody) {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "Else Body (ASTNode):" << std::endl; 
        elseBody->print(indent + 1);
    }
}

bool ConditionalNode::validate(IContext* context) {
    for (auto& branch : branches) {
        if (!branch.condition->validate(context)) {
            errorMessage = "Invalid condition in if/elif: " + branch.condition->getErrorMessage();
            return false;
        }
        
        if (!branch.body->validate(context)) {
            errorMessage = "Invalid body in if/elif: " + branch.body->getErrorMessage();
            return false;
        }
    }
    if (elseBody && !elseBody->validate(context)) { 
        errorMessage = "Invalid else body: " + elseBody->getErrorMessage();
        return false;
    }
    return true;
}

void ConditionalNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

ConditionalNode::~ConditionalNode() {
    for (auto& branch : branches) {
        delete branch.condition;
        delete branch.body; 
    }
    delete elseBody; 
}
