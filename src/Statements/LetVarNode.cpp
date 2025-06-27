#include "Statements/LetVarNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Context/Context.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Globals.hpp"
#include <iostream>

LetVarNode::LetVarNode(const std::string& id, ExpressionNode* e, ASTNode* b, Type* type)
    : identifier(id), expr(e), body(b), declaredType(type) {}



bool LetVarNode::validate(IContext* context) {
    if (!expr->validate(context)) {
        errorMessage = "Error in variable initialization '" + identifier + "': " +
                       expr->getErrorMessage();
        return false;
    }


    if (declaredType && declaredType != Type::getUnknownType()) {
        // Use inferType for comprehensive type checking
        Type* inferredType = expr->inferType(context);
        
        if (inferredType && inferredType != Type::getUnknownType()) {
            // Check if the inferred type matches the declared type
            if (inferredType->getTypeName() != declaredType->getTypeName()) {
                errorMessage = "Type error in variable '" + identifier + "': expected " +
                               declaredType->getTypeName() + " but got " + inferredType->getTypeName();
                return false;
            }
        }
    }

    
    IContext* childContext = context->CreateChildContext();
    Context* childCtx = dynamic_cast<Context*>(childContext);
    
    // Infer the type of the expression
    Type* inferredType = expr->inferType(context);
    
    // Add the variable to the context with its type
    if (childCtx) {
        // Use the declared type if provided, otherwise use the inferred type
        Type* varType = declaredType ? declaredType : inferredType;
        childCtx->DefineVariable(identifier, varType);
    } else {
        // Fallback to the old method if cast fails
        childContext->Define(identifier);
    }

    if (body && !body->validate(childContext)) {
        errorMessage = "Error in 'let' statement body: " + body->getErrorMessage();
        delete childContext;
        return false;
    }

    delete childContext;
    return true;
}

void LetVarNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

LetVarNode::~LetVarNode() {
    delete expr;
    delete body;
}
