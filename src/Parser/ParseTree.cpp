#include "Parser/ParseTree.hpp"
#include <iostream>

namespace Parser {

static void printNode(const ParseNode* node, int depth, bool isLast = true, std::string prefix = "");

void ParseTree::print() const {
    if (!root) return;
    
    std::cout << "Parse Tree:" << std::endl;
    printNode(root.get(), 0, true, "");
}

static void printNode(const ParseNode* node, int depth, bool isLast, std::string prefix) {
    if (!node) return;
    
    // Print current node with tree structure using ASCII characters
    std::cout << prefix;
    if (depth > 0) {
        std::cout << (isLast ? "+-- " : "|-- ");
    }
    
    // Print node value
    std::cout << node->symbol.value;
    
    // Print token info if it's a terminal
    if (node->symbol.isTerminal() && !node->token.lexeme.empty()) {
        std::cout << " [" << node->token.lexeme << "]";
    }
    
    std::cout << std::endl;
    
    // Print children with proper tree structure
    for (size_t i = 0; i < node->children.size(); ++i) {
        bool childIsLast = (i == node->children.size() - 1);
        std::string childPrefix = prefix;

        if (depth > 0) {
            childPrefix += (isLast ? "    " : "|   ");
        }

        printNode(node->children[i].get(), depth + 1, childIsLast, childPrefix);
    }
}

} // namespace Parser

