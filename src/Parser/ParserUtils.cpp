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





} // namespace Parser
