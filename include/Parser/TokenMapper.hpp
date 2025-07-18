#pragma once
#include <string>
#include "Lexer/Token.hpp"

namespace Parser {

class TokenMapper {
public:
    // Main method to get terminal symbol from token
    std::string getTerminalFromToken(const Token& token) const;

private:
    // Internal method to map token type to string
    std::string mapTokenTypeToString(TokenKind tokenKind) const;
};

} // namespace Parser
