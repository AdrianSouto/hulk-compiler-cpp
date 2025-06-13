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

int BlockExprNode::evaluate() const {
    if (expressions.empty()) {
        return 0;
    }
    
    
    for (size_t i = 0; i < expressions.size() - 1; i++) {
        expressions[i]->evaluate();
    }
    
    
    return expressions.back()->evaluate();
}

void BlockExprNode::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "ExprBlock {" << std::endl;
    for (auto expr : expressions) {
        expr->print(indent + 2);
    }
    std::cout << indentStr << "}" << std::endl;
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