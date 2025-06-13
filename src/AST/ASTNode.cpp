#include "AST/ASTNode.hpp"

const std::string& ASTNode::getErrorMessage() const {
    return errorMessage;
}

void ASTNode::setErrorMessage(const std::string& message) {
    errorMessage = message;
}
