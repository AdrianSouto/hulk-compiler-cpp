#include "Parser/ParserUtils.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Parser {

std::string ParserUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::unordered_set<std::string> ParserUtils::computeFirst(
    const std::vector<Symbol>& symbols,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& firstSets) {

    std::unordered_set<std::string> result;

    if (symbols.empty()) {
        result.insert("epsilon");
        return result;
    }

    bool allCanDeriveEpsilon = true;

    for (const auto& symbol : symbols) {
        auto it = firstSets.find(symbol.value);
        if (it != firstSets.end()) {
            // Add FIRST(symbol) - {epsilon} to result
            for (const auto& s : it->second) {
                if (s != "epsilon") {
                    result.insert(s);
                }
            }

            // Check if symbol can derive epsilon
            if (it->second.find("epsilon") == it->second.end()) {
                allCanDeriveEpsilon = false;
                break;
            }
        } else {
            // Symbol not found in firstSets (shouldn't happen)
            allCanDeriveEpsilon = false;
            break;
        }
    }

    // If all symbols can derive epsilon, add epsilon to result
    if (allCanDeriveEpsilon) {
        result.insert("epsilon");
    }

    return result;
}

bool ParserUtils::isLL1Grammar(
    const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& parsingTable) {

    // Check if there are any conflicts in the parsing table
    for (const auto& [nonTerminal, row] : parsingTable) {
        std::unordered_set<size_t> usedProductions;
        for (const auto& [terminal, production] : row) {
            if (usedProductions.find(production) != usedProductions.end()) {
                return false;
            }
            usedProductions.insert(production);
        }
    }
    return true;
}

std::vector<std::string> ParserUtils::findConflicts(
    const std::unordered_set<std::string>& nonTerminals,
    const std::vector<Production>& productions,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& firstSets,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& followSets) {

    std::vector<std::string> conflicts;

    // Check each non-terminal
    for (const auto& nonTerminal : nonTerminals) {
        std::unordered_map<std::string, std::vector<size_t>> entries;

        // Collect all productions for each terminal
        for (size_t i = 0; i < productions.size(); ++i) {
            if (productions[i].left.value == nonTerminal) {
                std::unordered_set<std::string> firstSet = computeFirst(productions[i].right, firstSets);

                for (const auto& terminal : firstSet) {
                    if (terminal != "epsilon") {
                        entries[terminal].push_back(i);
                    }
                }

                // If epsilon is in FIRST, add FOLLOW terminals
                if (firstSet.find("epsilon") != firstSet.end()) {
                    auto followIt = followSets.find(nonTerminal);
                    if (followIt != followSets.end()) {
                        for (const auto& terminal : followIt->second) {
                            entries[terminal].push_back(i);
                        }
                    }
                }
            }
        }

        // Check for conflicts
        for (const auto& [terminal, prods] : entries) {
            if (prods.size() > 1) {
                std::stringstream ss;
                ss << "Conflict at [" << nonTerminal << ", " << terminal << "]: productions ";
                for (size_t i = 0; i < prods.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << prods[i];
                }
                conflicts.push_back(ss.str());
            }
        }
    }

    return conflicts;
}

void ParserUtils::printFirstSets(
    const std::unordered_map<std::string, std::unordered_set<std::string>>& firstSets,
    const std::unordered_set<std::string>& nonTerminals) {

    std::cout << "FIRST sets:" << std::endl;
    for (const auto& [symbol, firstSet] : firstSets) {
        if (nonTerminals.find(symbol) != nonTerminals.end()) {
            std::cout << "  FIRST(" << symbol << ") = { ";
            bool first = true;
            for (const auto& s : firstSet) {
                if (!first) std::cout << ", ";
                std::cout << s;
                first = false;
            }
            std::cout << " }" << std::endl;
        }
    }
}

void ParserUtils::printFollowSets(
    const std::unordered_map<std::string, std::unordered_set<std::string>>& followSets) {

    std::cout << "FOLLOW sets:" << std::endl;
    for (const auto& [symbol, followSet] : followSets) {
        std::cout << "  FOLLOW(" << symbol << ") = { ";
        bool first = true;
        for (const auto& s : followSet) {
            if (!first) std::cout << ", ";
            std::cout << s;
            first = false;
        }
        std::cout << " }" << std::endl;
    }
}

void ParserUtils::printParsingTable(
    const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& parsingTable) {

    std::cout << "LL(1) Parsing Table:" << std::endl;
    for (const auto& [nonTerminal, row] : parsingTable) {
        std::cout << "  " << nonTerminal << ":" << std::endl;
        for (const auto& [terminal, production] : row) {
            std::cout << "    [" << terminal << "] -> production " << production << std::endl;
        }
    }
}

} // namespace Parser
