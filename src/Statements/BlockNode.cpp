#include "Statements/BlockNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>


BlockNode::BlockNode(const std::vector<StatementNode*>& stmts) : statements(stmts) {}

BlockNode::~BlockNode() {
    for (auto stmt : statements) {
        delete stmt;
    }
}



bool BlockNode::validate(IContext* context) {
    bool isValid = true;
    for (auto stmt : statements) {
        if (!stmt->validate(context)) {
            isValid = false;
        }
    }
    return isValid;
}


void BlockNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
