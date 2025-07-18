#include "Lexer/Token.hpp"
#include <map>

std::string tokenKindToString(TokenKind kind) {
    static const std::map<TokenKind, std::string> kindNames = {
        {TOKEN_NUMERIC, "NUMERIC"},
        {TOKEN_TEXT, "TEXT"},
        {TOKEN_NAME, "NAME"},
        {TOKEN_DECLARE, "DECLARE"},
        {TOKEN_WITHIN, "WITHIN"},
        {TOKEN_PROCEDURE, "PROCEDURE"},
        {TOKEN_CLASS, "CLASS"},
        {TOKEN_EXTENDS, "EXTENDS"},
        {TOKEN_CREATE, "CREATE"},
        {TOKEN_SUPER, "SUPER"},
        {TOKEN_WHEN, "WHEN"},
        {TOKEN_ELSEIF, "ELSEIF"},
        {TOKEN_OTHERWISE, "OTHERWISE"},
        {TOKEN_LOOP, "LOOP"},
        {TOKEN_ITERATE, "ITERATE"},
        {TOKEN_INSTANCEOF, "INSTANCEOF"},
        {TOKEN_CAST, "CAST"},
        {TOKEN_OUTPUT, "OUTPUT"},
        {TOKEN_AFFIRMATIVE, "AFFIRMATIVE"},
        {TOKEN_NEGATIVE, "NEGATIVE"},
        {TOKEN_KIND_NUMERIC, "KIND_NUMERIC"},
        {TOKEN_KIND_TEXT, "KIND_TEXT"},
        {TOKEN_KIND_LOGICAL, "KIND_LOGICAL"},
        {TOKEN_EQUAL, "EQUAL"},
        {TOKEN_NOTEQUAL, "NOTEQUAL"},
        {TOKEN_LESSEQUAL, "LESSEQUAL"},
        {TOKEN_GREATEREQUAL, "GREATEREQUAL"},
        {TOKEN_LOGICAL_AND, "LOGICAL_AND"},
        {TOKEN_LOGICAL_OR, "LOGICAL_OR"},
        {TOKEN_JOIN_SPACE, "JOIN_SPACE"},
        {TOKEN_IMPLIES, "IMPLIES"},
        {TOKEN_EXPONENT, "EXPONENT"},
        {TOKEN_BIND, "BIND"},
        {TOKEN_ADD, "ADD"},
        {TOKEN_SUBTRACT, "SUBTRACT"},
        {TOKEN_TIMES, "TIMES"},
        {TOKEN_QUOTIENT, "QUOTIENT"},
        {TOKEN_REMAINDER, "REMAINDER"},
        {TOKEN_JOIN, "JOIN"},
        {TOKEN_MATCH, "MATCH"},
        {TOKEN_BELOW, "BELOW"},
        {TOKEN_ABOVE, "ABOVE"},
        {TOKEN_NEGATE, "NEGATE"},
        {TOKEN_OPEN_PAREN, "OPEN_PAREN"},
        {TOKEN_CLOSE_PAREN, "CLOSE_PAREN"},
        {TOKEN_OPEN_BRACE, "OPEN_BRACE"},
        {TOKEN_CLOSE_BRACE, "CLOSE_BRACE"},
        {TOKEN_TERMINATOR, "TERMINATOR"},
        {TOKEN_SEPARATOR, "SEPARATOR"},
        {TOKEN_MARKER, "MARKER"},
        {TOKEN_ACCESSOR, "ACCESSOR"},
        {TOKEN_ENDFILE, "ENDFILE"},
        {TOKEN_INVALID, "INVALID"},
        {TOKEN_UNDEFINED, "UNDEFINED"}
    };
    
    auto it = kindNames.find(kind);
    return (it != kindNames.end()) ? it->second : "UNKNOWN";
}

TokenKind getTokenCategory(TokenKind token) {

    if (token >= TOKEN_DECLARE && token <= TOKEN_KIND_LOGICAL) {
        return TOKEN_NAME;
    }
    return token;
}

int getTokenPrecedence(TokenKind token) {
    static const std::map<TokenKind, int> precedenceMap = {
        {TOKEN_LOGICAL_OR, 1},
        {TOKEN_LOGICAL_AND, 2},
        {TOKEN_EQUAL, 3},
        {TOKEN_NOTEQUAL, 3},
        {TOKEN_BELOW, 4},
        {TOKEN_ABOVE, 4},
        {TOKEN_LESSEQUAL, 4},
        {TOKEN_GREATEREQUAL, 4},
        {TOKEN_ADD, 5},
        {TOKEN_SUBTRACT, 5},
        {TOKEN_JOIN, 5},
        {TOKEN_JOIN_SPACE, 5},
        {TOKEN_TIMES, 6},
        {TOKEN_QUOTIENT, 6},
        {TOKEN_REMAINDER, 6},
        {TOKEN_EXPONENT, 7},
        {TOKEN_NEGATE, 8}
    };
    
    auto it = precedenceMap.find(token);
    return (it != precedenceMap.end()) ? it->second : 0;
}

bool hasHigherPriority(TokenKind token1, TokenKind token2) {
    return getTokenPrecedence(token1) > getTokenPrecedence(token2);
}