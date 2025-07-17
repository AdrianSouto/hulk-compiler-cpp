#pragma once
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <stack>
#include <fstream>
#include "Symbol.hpp"
#include "ParseTree.hpp"
#include "Lexer/Token.hpp"

namespace Parser {

class LL1Parser {
public:
    // Core grammar data - using more efficient containers
    Symbol startSymbol;
    std::unordered_set<std::string> terminals;
    std::unordered_set<std::string> nonTerminals;
    std::vector<Production> productions;
    
    // LL(1) parsing tables - using hash maps for better performance
    std::unordered_map<std::string, std::unordered_set<std::string>> firstSets;
    std::unordered_map<std::string, std::unordered_set<std::string>> followSets;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> parsingTable;
    
    // Cache for token to symbol mapping for performance
    mutable std::unordered_map<int, std::string> tokenSymbolCache;

    // Grammar loading helper methods
    void parseGrammarFile(std::ifstream& file, std::vector<std::string>& productionLines);
    void parseTerminals(const std::string& line);
    void parseNonTerminals(const std::string& line);
    void processProductions(const std::vector<std::string>& productionLines);
    void processProductionLine(const std::string& prodLine);
    std::vector<std::string> splitAlternatives(const std::string& rhsStr);
    std::vector<Symbol> parseRightHandSide(const std::string& altStr);
    SymbolType determineSymbolType(const std::string& token);
    void initializeGrammar();
    void setStartSymbol();

    // FIRST and FOLLOW calculation methods
    void calculateFirst();
    void initializeFirstSets();
    void computeFirstSetsIteratively();
    void calculateFollow();
    void buildParsingTable();
    std::unordered_set<std::string> computeFirst(const std::vector<Symbol>& symbols) const;

    // Parsing helper methods
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
    
    // Utility methods
    static std::string trim(const std::string& str);
    std::string getTerminalFromToken(const Token& token) const;

    // Constructor
    LL1Parser() = default;

    // Load grammar from file
    static LL1Parser loadFromFile(const std::string& filename);

    // Parse tokens into parse tree
    std::unique_ptr<ParseTree> parse(const std::vector<Token>& tokens);
    
    // Validation
    bool isLL1() const;
    std::vector<std::string> getConflicts() const;
    
    // Debug
    void printFirst() const;
    void printFollow() const;
    void printParsingTable() const;
};

} // namespace Parser

