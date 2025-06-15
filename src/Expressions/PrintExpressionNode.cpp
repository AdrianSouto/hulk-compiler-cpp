#include "../../include/Expressions/PrintExpressionNode.hpp"
#include "../../include/Visitors/LLVMCodegenVisitor.hpp"
#include "../../include/Context/IContext.hpp"
#include <iostream>

PrintExpressionNode::PrintExpressionNode(ExpressionNode* expr) : expression(expr) {}

PrintExpressionNode::~PrintExpressionNode() {
    delete expression;
}

void PrintExpressionNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

void PrintExpressionNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "PrintExpression:" << std::endl;
    if (expression) {
        expression->print(indent + 2);
    }
}

bool PrintExpressionNode::validate(IContext* context) {
    if (!expression) {
        setErrorMessage("Print expression has no expression to print");
        return false;
    }
    
    if (!expression->validate(context)) {
        setErrorMessage("Error in print expression: " + expression->getErrorMessage());
        return false;
    }
    
    return true;
}

int PrintExpressionNode::evaluate() const {
    if (expression) {
        return expression->evaluate();
    }
    return 0;
}

double PrintExpressionNode::evaluateDouble() const {
    if (expression) {
        return expression->evaluateDouble();
    }
    return 0.0;
}

std::string PrintExpressionNode::evaluateString() const {
    if (expression) {
        return expression->evaluateString();
    }
    return "";
}