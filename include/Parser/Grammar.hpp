#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>
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

    // Helper methods
    void calculateFirst();
    void calculateFollow();
    void buildParsingTable();
    std::set<Symbol> computeFirst(const std::vector<Symbol>& symbols) const;
    
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