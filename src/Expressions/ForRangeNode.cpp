#include "Expressions/ForRangeNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include "AST/StatementNode.hpp"
#include <iostream>

ForRangeNode::ForRangeNode(const std::string& var, ExpressionNode* start, ExpressionNode* end, ASTNode* b)
    : loopVar(var), startExpr(start), endExpr(end), body(b) {}

int ForRangeNode::evaluate() const {
    int start = startExpr->evaluate();
    int end = endExpr->evaluate();
    int lastValue = 0;
    
    
    std::string originalValue;
    bool hadOriginalValue = false;
    if (variables.find(loopVar) != variables.end()) {
        originalValue = variables[loopVar];
        hadOriginalValue = true;
    }
    
    
    for (int i = start; i < end; i++) {
        variables[loopVar] = std::to_string(i);
        
        if (auto* exprBody = dynamic_cast<ExpressionNode*>(body)) {
            lastValue = exprBody->evaluate();
        } else if (auto* stmtBody = dynamic_cast<StatementNode*>(body)) {
            stmtBody->execute();
            lastValue = 0; 
        }
    }
    
    
    if (hadOriginalValue) {
        variables[loopVar] = originalValue;
    } else {
        variables.erase(loopVar);
    }
    
    return lastValue;
}

void ForRangeNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "ForRange: " << loopVar << " in range(" << std::endl;
    
    startExpr->print(indent + 1);
    std::cout << ", ";
    endExpr->print(indent + 1);
    std::cout << ")" << std::endl;
    
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Body:" << std::endl;
    
    if (auto* exprBody = dynamic_cast<ExpressionNode*>(body)) {
        exprBody->print(indent + 1);
    } else if (auto* stmtBody = dynamic_cast<StatementNode*>(body)) {
        stmtBody->print(indent + 1);
    }
}

bool ForRangeNode::validate(IContext* context) {
    
    if (!startExpr->validate(context)) {
        errorMessage = "Error in for loop start expression: " + startExpr->getErrorMessage();
        return false;
    }
    
    if (!endExpr->validate(context)) {
        errorMessage = "Error in for loop end expression: " + endExpr->getErrorMessage();
        return false;
    }
    
    
    IContext* loopContext = context->CreateChildContext();
    loopContext->Define(loopVar);
    
    
    bool bodyValid = false;
    if (auto* exprBody = dynamic_cast<ExpressionNode*>(body)) {
        bodyValid = exprBody->validate(loopContext);
        if (!bodyValid) {
            errorMessage = "Error in for loop body: " + exprBody->getErrorMessage();
        }
    } else if (auto* stmtBody = dynamic_cast<StatementNode*>(body)) {
        bodyValid = stmtBody->validate(loopContext);
        if (!bodyValid) {
            errorMessage = "Error in for loop body: " + stmtBody->getErrorMessage();
        }
    }
    
    delete loopContext;
    return bodyValid;
}

void ForRangeNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

ForRangeNode::~ForRangeNode() {
    delete startExpr;
    delete endExpr;
    delete body;
}