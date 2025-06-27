#include "Expressions/ForRangeNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include "AST/StatementNode.hpp"
#include "Statements/ExpressionStatementNode.hpp"
#include "Expressions/AssignmentNode.hpp"
#include <iostream>

ForRangeNode::ForRangeNode(const std::string& var, ExpressionNode* start, ExpressionNode* end, ASTNode* b)
    : loopVar(var), startExpr(start), endExpr(end), body(b) {}



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