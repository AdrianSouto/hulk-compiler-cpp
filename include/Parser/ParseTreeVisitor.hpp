#pragma once

// Forward declarations
class ASTNode;

namespace Parser {

class ParseNode;

class ParseTreeVisitor {
public:
    virtual ~ParseTreeVisitor() = default;
    virtual ASTNode* visit(ParseNode* node) = 0;
};

} // namespace Parser