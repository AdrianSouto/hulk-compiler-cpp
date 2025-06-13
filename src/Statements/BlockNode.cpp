#include "Statements/BlockNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>


BlockNode::BlockNode(const std::vector<StatementNode*>& stmts) : statements(stmts) {}

BlockNode::~BlockNode() {
    for (auto stmt : statements) {
        delete stmt;
    }
}

void BlockNode::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "Block {" << std::endl;
    for (auto stmt : statements) {
        stmt->print(indent + 2);
    }
    std::cout << indentStr << "}" << std::endl;
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

void BlockNode::execute() const {
    for (auto stmt : statements) {
        stmt->execute();
    }
}

void BlockNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}
