#include "Expressions/LetExprNode.hpp"
#include "Context/Context.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp" 
#include "Globals.hpp"
#include "Types/Type.hpp"
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
    Context* childCtx = dynamic_cast<Context*>(childContext);
    std::vector<std::string> errors;
    bool hasErrors = false;
    
    // Validate each declaration and add it to the context sequentially
    for (const auto& decl : declarations) {
        // Validate the expression in the current context
        if (!decl.expr->validate(childContext)) {
            errors.push_back("Variable '" + decl.id + "': " + decl.expr->getErrorMessage());
            hasErrors = true;
            // Still add the variable to context to continue checking other errors
        }
        
        // Infer the type of the expression
        Type* exprType = decl.expr->inferType(childContext);
        
        // Check type compatibility if type annotation is provided
        if (decl.type && decl.type->getTypeName() != "Unknown") {
            if (exprType && exprType->getTypeName() != "Unknown") {
                // Check if the expression type matches the declared type
                if (exprType->getTypeName() != decl.type->getTypeName()) {
                    errors.push_back("Type mismatch for variable '" + decl.id + 
                                   "': expected " + decl.type->getTypeName() + 
                                   " but got " + exprType->getTypeName());
                    hasErrors = true;
                }
            }
        }
        
        // Add the variable to the context with its type
        if (childCtx) {
            // Use the declared type if provided, otherwise use the inferred type
            Type* varType = decl.type ? decl.type : exprType;
            childCtx->DefineVariable(decl.id, varType);
        } else {
            // Fallback to the old method if cast fails
            childContext->Define(decl.id);
        }
    }
    
    // Validate the body with all variables defined
    bool bodyValid = body->validate(childContext);
    if (!bodyValid) {
        errors.push_back("In body: " + body->getErrorMessage());
        hasErrors = true;
    }
    
    // Construct error message
    if (hasErrors) {
        errorMessage = "Errors in let expression:";
        for (const auto& error : errors) {
            errorMessage += "\n  - " + error;
        }
    }
    
    delete childContext;
    return !hasErrors;
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