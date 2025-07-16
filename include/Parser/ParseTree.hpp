#pragma once
#include <vector>
#include <memory>
#include "Symbol.hpp"
#include "Lexer/Token.hpp"

namespace Parser {

class ParseTreeVisitor;

class ParseNode {
public:
    Symbol symbol;
    Token token;
    std::vector<std::unique_ptr<ParseNode>> children;
    ParseNode* parent;

    ParseNode(const Symbol& s) : symbol(s), parent(nullptr) {}
    
    void addChild(std::unique_ptr<ParseNode> child) {
        child->parent = this;
        children.push_back(std::move(child));
    }

    // Accept visitor pattern
    class ASTNode* accept(ParseTreeVisitor& visitor);
};

class ParseTree {
public:
    std::unique_ptr<ParseNode> root;

    ParseTree() {}
    ParseTree(std::unique_ptr<ParseNode> r) : root(std::move(r)) {}

    // Accept visitor pattern
    class ASTNode* accept(ParseTreeVisitor& visitor);

    void print(int depth = 0) const;
};

} // namespace Parser