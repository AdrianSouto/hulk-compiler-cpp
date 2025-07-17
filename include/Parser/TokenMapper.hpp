#pragma once
#include <string>
#include <unordered_map>
#include "Lexer/Token.hpp"

namespace Parser {

class TokenMapper {
public:
    // Cache for token to symbol mapping for performance
    mutable std::unordered_map<int, std::string> tokenSymbolCache;

    // Main method to get terminal symbol from token
    std::string getTerminalFromToken(const Token& token) const;

    // Clear cache if needed
    void clearCache();

private:
    // Internal method to map token type to string
    std::string mapTokenTypeToString(TokenType tokenType) const;
};

} // namespace Parser
