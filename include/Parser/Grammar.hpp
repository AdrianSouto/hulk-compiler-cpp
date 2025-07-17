#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>
#include <stack>
#include <fstream>
#include "Symbol.hpp"
#include "ParseTree.hpp"
#include "Lexer/Token.hpp"

namespace Parser {

class Grammar {
private:
    Symbol startSymbol;
    std::set<Symbol> terminals;
    std::set<Symbol> nonTerminals;
    std::vector<Production> productions;
    
    // LL(1) parsing tables
    std::map<Symbol, std::set<Symbol>> firstSets;
    std::map<Symbol, std::set<Symbol>> followSets;
    std::map<Symbol, std::map<Symbol, int>> parsingTable;

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
    std::set<Symbol> computeFirst(const std::vector<Symbol>& symbols) const;

    // Parsing helper methods
    void initializeParsingStack(std::stack<ParseNode*>& stack, ParseNode* root);
    void processNonTerminal(ParseNode* node, const std::vector<Token>& tokens, 
                           size_t& tokenIndex, std::stack<ParseNode*>& stack);
    void processTerminal(ParseNode* node, const std::vector<Token>& tokens, 
                        size_t& tokenIndex, std::stack<ParseNode*>& stack);
    Token getLookaheadToken(const std::vector<Token>& tokens, size_t index) const;
    int findProductionInTable(const Symbol& nonTerminal, const Symbol& terminal, 
                             const Token& lookahead) const;
    void expandNonTerminal(ParseNode* node, const Production& prod, 
                          std::stack<ParseNode*>& stack);
    void validateParsingCompletion(const std::vector<Token>& tokens, size_t tokenIndex) const;
    
    // Error handling methods
    void throwUnexpectedTokenError(const Token& lookahead) const;
    void throwTerminalMismatchError(const Symbol& expected, const Token& found) const;
    
    // Utility methods
    static std::string trim(const std::string& str);
    Symbol getTerminalFromToken(const Token& token) const;

public:
    Grammar() {}
    
    // Load grammar from file
    static Grammar loadFromFile(const std::string& filename);
    
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