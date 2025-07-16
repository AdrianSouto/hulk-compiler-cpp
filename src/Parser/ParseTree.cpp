#include "Parser/ParseTree.hpp"
#include "Parser/ParseTreeVisitor.hpp"
#include <iostream>

namespace Parser {

ASTNode* ParseNode::accept(ParseTreeVisitor& visitor) {
    return visitor.visit(this);
}

ASTNode* ParseTree::accept(ParseTreeVisitor& visitor) {
    if (root) {
        return root->accept(visitor);
    }
    return nullptr;
}

static void printNode(const ParseNode* node, int depth);

void ParseTree::print(int depth) const {
    if (!root) return;
    
    printNode(root.get(), depth);
}

static void printNode(const ParseNode* node, int depth) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    
    // Print node value
    std::cout << node->symbol.value;
    
    // Print token info if it's a terminal
    if (node->symbol.isTerminal() && !node->token.lexeme.empty()) {
        std::cout << " [" << node->token.lexeme << "]";
    }
    
    std::cout << std::endl;
    
    // Print children
    for (const auto& child : node->children) {
        printNode(child.get(), depth + 1);
    }
}

} // namespace Parser