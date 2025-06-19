#include "Expressions/LetExprNode.hpp"
#include "Context/Context.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp" 
#include "Globals.hpp"
#include <stdexcept>
#include <iostream>
#include <map>

LetExprNode::LetExprNode(const std::vector<VarDeclPair>& decls, ExpressionNode* expr)
    : declarations(decls), body(expr) {}

LetExprNode::~LetExprNode() {
    for (const auto& decl : declarations) {
        delete decl.expr;
    }
    delete body;
}

int LetExprNode::evaluate() const {
    // Save original values of variables that will be shadowed
    std::map<std::string, std::string> savedValues;
    for (const auto& decl : declarations) {
        if (variables.find(decl.id) != variables.end()) {
            savedValues[decl.id] = variables[decl.id];
        }
    }
    
    // Evaluate and set the let-bound variables
    for (const auto& decl : declarations) {
        int value = decl.expr->evaluate();
        variables[decl.id] = std::to_string(value);
    }
    
    // Evaluate the body
    int result = body->evaluate();
    
    // Restore original values or remove variables
    for (const auto& decl : declarations) {
        if (savedValues.find(decl.id) != savedValues.end()) {
            variables[decl.id] = savedValues[decl.id];
        } else {
            variables.erase(decl.id);
        }
    }
    
    return result;
}

std::string LetExprNode::evaluateString() const {
    
    
    return std::to_string(evaluate());
}

bool LetExprNode::validate(IContext* context) {
    // Create child context for the let expression
    IContext* childContext = context->CreateChildContext();
    
    // Validate each declaration and add it to the context sequentially
    for (const auto& decl : declarations) {
        // Validate the expression in the current context
        if (!decl.expr->validate(childContext)) {
            errorMessage = "Error in declaration of '" + decl.id + "': " + decl.expr->getErrorMessage();
            delete childContext;
            return false;
        }
        
        // Add the variable to the context so it's available for subsequent declarations
        childContext->Define(decl.id);
    }
    
    // Validate the body with all variables defined
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