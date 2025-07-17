#pragma once
#include <memory>
#include <vector>
#include <string>
#include "LL1Parser.hpp"
#include "ParseTree.hpp"
#include "Lexer/Token.hpp"

// Forward declaration
class Program;

namespace Parser {

class Parser {
private:
    LL1Parser grammar;
    std::vector<std::string> errors;

public:
    Parser();
    
    // Parse tokens into AST
    Program* parse(const std::vector<Token>& tokens);
    
    // Parse tokens and return parse tree (for debugging)
    std::unique_ptr<ParseTree> parseToTree(const std::vector<Token>& tokens);
    
    // Error handling
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // Debug
    void printGrammarInfo() const;
    void printParseTree(const std::vector<Token>& tokens) const;
};

} // namespace Parser

