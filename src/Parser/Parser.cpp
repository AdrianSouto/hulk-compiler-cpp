#include "Parser/Parser.hpp"
#include <iostream>
#include <sstream>

namespace Parser {

Parser::Parser() {
    try {
        // Load grammar from file
        grammar = LL1Parser::loadFromFile("grammar.txt");

        // Check if grammar is LL(1)
        if (!grammar.isLL1()) {
            errors.push_back("Warning: Grammar is not LL(1), parsing may fail");
            auto conflicts = grammar.getConflicts();
            for (const auto& conflict : conflicts) {
                errors.push_back("  " + conflict);
            }
        }
    } catch (const std::exception& e) {
        errors.push_back("Failed to load grammar: " + std::string(e.what()));
    }
}

std::unique_ptr<ParseTree> Parser::parse(const std::vector<Token>& tokens) {
    clearErrors();
    
    if (!errors.empty()) {
        // Grammar loading failed
        return nullptr;
    }
    
    try {
        // Parse tokens into parse tree
        auto parseTree = grammar.parse(tokens);
        
        if (!parseTree) {
            errors.push_back("Failed to build parse tree");
            return nullptr;
        }
        
        return parseTree;
        
    } catch (const std::exception& e) {
        errors.push_back(e.what());
        return nullptr;
    }
}

void Parser::printParseTree(const std::vector<Token>& tokens) const {
    // Create a temporary parser instance to avoid modifying const state
    Parser tempParser;
    
    auto parseTree = tempParser.parse(tokens);

    if (!parseTree) {
        std::cout << "Failed to create parse tree:" << std::endl;
        for (const auto& error : tempParser.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
        return;
    }
    
    std::cout << "=== Parse Tree ===" << std::endl;
    parseTree->print();
    std::cout << "=================" << std::endl;
}

void Parser::printGrammarInfo() const {
    std::cout << "=== Grammar Information ===" << std::endl;
    
    if (!grammar.isLL1()) {
        std::cout << "Warning: Grammar is not LL(1)" << std::endl;
        auto conflicts = grammar.getConflicts();
        for (const auto& conflict : conflicts) {
            std::cout << "  " << conflict << std::endl;
        }
        std::cout << std::endl;
    }
    
    grammar.printFirst();
    std::cout << std::endl;
    
    grammar.printFollow();
    std::cout << std::endl;
    
    grammar.printParsingTable();
}

} // namespace Parser

