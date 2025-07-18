#pragma once
#include <memory>
#include <vector>
#include <string>
#include "LL1Parser.hpp"
#include "ParseTree.hpp"
#include "Lexer/Token.hpp"

namespace Parser {

class Parser {
private:
    LL1Parser grammar;
    std::vector<std::string> errors;

public:
    Parser();
    
    // Parse tokens and return parse tree - simplified interface
    std::unique_ptr<ParseTree> parse(const std::vector<Token>& tokens);

    // Error handling
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
};

} // namespace Parser

