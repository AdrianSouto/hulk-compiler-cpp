#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Grammar.hpp"
#include "ParseTree.hpp"
#include "Lexer/Token.hpp"

// Forward declaration
class Program;

namespace Parser {

class Parser {
private:
    Grammar grammar;
    std::vector<std::string> errors;

public:
    Parser();
    
    // Parse tokens into AST
    Program* parse(const std::vector<Token>& tokens);
    
    // Error handling
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // Debug
    void printGrammarInfo() const;
};

} // namespace Parser