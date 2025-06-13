#include "Expressions/LetExprNode.hpp"
#include "Context/Context.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp" 
#include <stdexcept>
#include <iostream>

LetExprNode::LetExprNode(const std::vector<VarDeclPair>& decls, ExpressionNode* expr)
    : declarations(decls), body(expr) {}

LetExprNode::~LetExprNode() {
    for (const auto& decl : declarations) {
        delete decl.expr;
    }
    delete body;
}

int LetExprNode::evaluate() const {
    
    Context context;
    
    
    for (const auto& decl : declarations) {
        int value = decl.expr->evaluate();
        context.Define(decl.id);
        
    }
    
    
    return body->evaluate();
}

std::string LetExprNode::evaluateString() const {
    
    
    return std::to_string(evaluate());
}

bool LetExprNode::validate(IContext* context) {
    
    for (const auto& decl : declarations) {
        if (!decl.expr->validate(context)) {
            errorMessage = "Error in declaration of '" + decl.id + "': " + decl.expr->getErrorMessage();
            return false;
        }
    }
    
    
    IContext* childContext = context->CreateChildContext();
    
    
    for (const auto& decl : declarations) {
        childContext->Define(decl.id);
    }
    
    
    bool result = body->validate(childContext);
    if (!result) {
        errorMessage = "Error in let expression body: " + body->getErrorMessage();
    }
    
    delete childContext;
    return result;
}

void LetExprNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}


void LetExprNode::print(int indent) const {
    std::string indentation(indent, ' ');
    std::cout << indentation << "LetExprNode:" << std::endl;
    
    std::cout << indentation << "  Declarations:" << std::endl;
    for (const auto& decl : declarations) {
        std::cout << indentation << "    " << decl.id << " = " << std::endl;
        decl.expr->print(indent + 6);
    }
    
    std::cout << indentation << "  Body:" << std::endl;
    body->print(indent + 4);
}
