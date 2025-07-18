#include "Parser/Parser.hpp"
#include <iostream>

namespace Parser {

Parser::Parser() {
    try {
        grammar = LL1Parser::loadFromFile("grammars");
    }
    catch (const std::exception& e)
    {
        errors.push_back("Failed to load grammar: " + std::string(e.what()));
    }
}

std::unique_ptr<ParseTree> Parser::parse(const std::vector<Token>& tokens) {
    clearErrors();
    if (!errors.empty()) return nullptr;
    
    try {
        return grammar.parse(tokens);
    } catch (const std::exception& e) {
        errors.push_back(e.what());
        return nullptr;
    }
}

} // namespace Parser

