#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <stack>
#include "Symbol.hpp"
#include "ParseTree.hpp"
#include "Lexer/Token.hpp"
#include "TokenMapper.hpp"
#include "ParserUtils.hpp"

namespace Parser {

class LL1Parser {
public:
    // Constructor
    LL1Parser() = default;

    // Load grammar from directory
    static LL1Parser loadFromFile(const std::string& grammarDir);

    // Main parsing method
    std::unique_ptr<ParseTree> parse(const std::vector<Token>& tokens);
    
    // Validation methods (using ParserUtils)
    bool isLL1() const;
    std::vector<std::string> getConflicts() const;
    
    // Debug methods (using ParserUtils)
    void printFirst() const;
    void printFollow() const;
    void printParsingTable() const;

private:
    // Core grammar data - using more efficient containers
    Symbol startSymbol;
    std::unordered_set<std::string> terminals;
    std::unordered_set<std::string> nonTerminals;
    std::vector<Production> productions;
    
    // LL(1) parsing tables - using hash maps for better performance
    std::unordered_map<std::string, std::unordered_set<std::string>> firstSets;
    std::unordered_map<std::string, std::unordered_set<std::string>> followSets;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> parsingTable;
    
    // Helper components
    TokenMapper tokenMapper;

    // Grammar loading methods
    void loadGrammarFromDirectory(const std::string& grammarDir);
    void loadTerminals(const std::string& filename);
    void loadNonTerminals(const std::string& filename);
    void loadProductions(const std::string& filename);
    void parseProduction(const std::string& prodLine);

    // FIRST and FOLLOW calculation methods
    void calculateFirst();
    void initializeFirstSets();
    void computeFirstSetsIteratively();
    void calculateFollow();
    void buildParsingTable();
    void initializeGrammar();

    // Parsing helper methods (core algorithm)
    void initializeParsingStack(std::stack<ParseNode*>& stack, ParseNode* root);
    void processNonTerminal(ParseNode* node, const std::vector<Token>& tokens, 
                           size_t& tokenIndex, std::stack<ParseNode*>& stack);
    void processTerminal(ParseNode* node, const std::vector<Token>& tokens, 
                        size_t& tokenIndex, std::stack<ParseNode*>& stack);
    Token getLookaheadToken(const std::vector<Token>& tokens, size_t index) const;
    size_t findProductionInTable(const std::string& nonTerminal, const std::string& terminal, 
                                const Token& lookahead) const;
    void expandNonTerminal(ParseNode* node, const Production& prod, 
                          std::stack<ParseNode*>& stack);
    void validateParsingCompletion(const std::vector<Token>& tokens, size_t tokenIndex) const;
    
    // Error handling methods
    void throwUnexpectedTokenError(const Token& lookahead) const;
    void throwTerminalMismatchError(const std::string& expected, const Token& found) const;
};

} // namespace Parser

