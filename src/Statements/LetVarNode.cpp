#include "Statements/LetVarNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Globals.hpp"
#include <iostream>

LetVarNode::LetVarNode(const std::string& id, ExpressionNode* e, ASTNode* b, Type* type)
    : identifier(id), expr(e), body(b), declaredType(type) {}

void LetVarNode::execute() const {
    std::string oldValue;
    bool hadValue = false;
    if (variables.find(identifier) != variables.end()) {
        oldValue = variables[identifier];
        hadValue = true;
    }

    variables[identifier] = expr->evaluateString();


    if (auto stmt = dynamic_cast<StatementNode*>(body)) {
        stmt->execute();
    } else if (auto exprBody = dynamic_cast<ExpressionNode*>(body)) {
        exprBody->evaluateString();
    }

    if (hadValue) {
        variables[identifier] = oldValue;
    } else {
        variables.erase(identifier);
    }
}

void LetVarNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "LetVar: " << identifier;


    if (declaredType != Type::getUnknownType()) {
        std::cout << ": " << declaredType->toString();
    }

    std::cout << std::endl;

    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "Value: ";
    expr->print(indent + 2);

    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "Body: ";
    if (body) body->print(indent + 2);
}

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
    
    
    childContext->Define(identifier);

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
