#include "Statements/PrintStatementNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include <iostream>

PrintStatementNode::PrintStatementNode(ExpressionNode* expr) : expression(expr) {}

void PrintStatementNode::execute() const {
    std::cout << "Print: " << expression->evaluate() << std::endl;
}

void PrintStatementNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "PrintStatement:" << std::endl;
    expression->print(indent + 1);
}

bool PrintStatementNode::validate(IContext* context) {
    if (!expression->validate(context)) {
        errorMessage = "Error in print statement: " + expression->getErrorMessage();
        return false;
    }
    return true;
}

void PrintStatementNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

PrintStatementNode::~PrintStatementNode() {
    delete expression;
}

