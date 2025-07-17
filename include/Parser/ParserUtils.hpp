#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Symbol.hpp"

namespace Parser {

class ParserUtils {
public:
    // String utility methods
    static std::string trim(const std::string& str);

    // FIRST and FOLLOW set calculation utilities
    static std::unordered_set<std::string> computeFirst(
        const std::vector<Symbol>& symbols,
        const std::unordered_map<std::string, std::unordered_set<std::string>>& firstSets
    );

    // Validation utilities
    static bool isLL1Grammar(
        const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& parsingTable
    );

    static std::vector<std::string> findConflicts(
        const std::unordered_set<std::string>& nonTerminals,
        const std::vector<Production>& productions,
        const std::unordered_map<std::string, std::unordered_set<std::string>>& firstSets,
        const std::unordered_map<std::string, std::unordered_set<std::string>>& followSets
    );

    // Debug printing utilities
    static void printFirstSets(
        const std::unordered_map<std::string, std::unordered_set<std::string>>& firstSets,
        const std::unordered_set<std::string>& nonTerminals
    );

    static void printFollowSets(
        const std::unordered_map<std::string, std::unordered_set<std::string>>& followSets
    );

    static void printParsingTable(
        const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& parsingTable
    );
};

} // namespace Parser
