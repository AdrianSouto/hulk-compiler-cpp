#include "Expressions/BlockExprNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include <iostream>

BlockExprNode::BlockExprNode(const std::vector<ExpressionNode*>& exprs) 
    : expressions(exprs) {}

BlockExprNode::~BlockExprNode() {
    for (auto expr : expressions) {
        delete expr;
    }
}


bool BlockExprNode::validate(IContext* context) {
    for (auto expr : expressions) {
        if (!expr || !expr->validate(context)) {
            return false;
        }
    }
    return true;
}

void BlockExprNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}