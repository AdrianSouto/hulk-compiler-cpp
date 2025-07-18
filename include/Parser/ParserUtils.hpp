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



};

} // namespace Parser
