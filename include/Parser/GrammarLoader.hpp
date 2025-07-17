#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include "Symbol.hpp"

namespace Parser {

class GrammarLoader {
public:
    // Core grammar data that will be populated
    std::unordered_set<std::string> terminals;
    std::unordered_set<std::string> nonTerminals;
    std::vector<Production> productions;
    Symbol startSymbol;

    // Main loading method
    bool loadFromFile(const std::string& filename);

    // Grammar file parsing methods
    void parseGrammarFile(std::ifstream& file, std::vector<std::string>& productionLines);
    void parseTerminals(const std::string& line);
    void parseNonTerminals(const std::string& line);
    void processProductions(const std::vector<std::string>& productionLines);
    void processProductionLine(const std::string& prodLine);

    // Helper methods for parsing
    std::vector<std::string> splitAlternatives(const std::string& rhsStr);
    std::vector<Symbol> parseRightHandSide(const std::string& altStr);
    SymbolType determineSymbolType(const std::string& token);
    void setStartSymbol();

private:
    // Internal helper method
    void initializeAfterLoading();
};

} // namespace Parser
