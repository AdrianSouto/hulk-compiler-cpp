#pragma once
#include <string>

enum TokenKind {
    // Literals and identifiers
    TOKEN_NUMERIC,
    TOKEN_TEXT,
    TOKEN_NAME,
    
    // Keywords
    TOKEN_DECLARE,
    TOKEN_WITHIN,
    TOKEN_PROCEDURE,
    TOKEN_CLASS,
    TOKEN_EXTENDS,
    TOKEN_CREATE,
    TOKEN_SUPER,
    TOKEN_WHEN,
    TOKEN_ELSEIF,
    TOKEN_OTHERWISE,
    TOKEN_LOOP,
    TOKEN_ITERATE,
    TOKEN_INSTANCEOF,
    TOKEN_CAST,
    TOKEN_OUTPUT,
    TOKEN_AFFIRMATIVE,
    TOKEN_NEGATIVE,
    
    // Type keywords
    TOKEN_KIND_NUMERIC,
    TOKEN_KIND_TEXT,
    TOKEN_KIND_LOGICAL,
    
    // Multi-character operators
    TOKEN_EQUAL,         // ==
    TOKEN_NOTEQUAL,      // !=
    TOKEN_LESSEQUAL,     // <=
    TOKEN_GREATEREQUAL,  // >=
    TOKEN_LOGICAL_AND,   // &&
    TOKEN_LOGICAL_OR,    // |
    TOKEN_JOIN_SPACE,    // @@
    TOKEN_IMPLIES,       // =>
    TOKEN_EXPONENT,      // **
    TOKEN_BIND,          // :=
    
    // Single character operators
    TOKEN_ADD,           // +
    TOKEN_SUBTRACT,      // -
    TOKEN_TIMES,         // *
    TOKEN_QUOTIENT,      // /
    TOKEN_REMAINDER,     // %
    TOKEN_JOIN,          // @
    TOKEN_MATCH,         // =
    TOKEN_BELOW,         // <
    TOKEN_ABOVE,         // >
    TOKEN_NEGATE,        // !
    
    // Delimiters
    TOKEN_OPEN_PAREN,    // (
    TOKEN_CLOSE_PAREN,   // )
    TOKEN_OPEN_BRACE,    // {
    TOKEN_CLOSE_BRACE,   // }
    TOKEN_TERMINATOR,    // ;
    TOKEN_SEPARATOR,     // ,
    TOKEN_MARKER,        // :
    TOKEN_ACCESSOR,      // .
    
    // Special tokens
    TOKEN_ENDFILE,
    TOKEN_INVALID,
    TOKEN_UNDEFINED
};

struct Token {
    std::string text;
    TokenKind kind;
    int row;
    int col;
    
    // For numeric values
    union {
        int wholeValue;
        double realValue;
    };
    
    Token(const std::string& txt, TokenKind k, int r, int c) 
        : text(txt), kind(k), row(r), col(c), wholeValue(0) {}
    
    Token() : kind(TOKEN_UNDEFINED), row(0), col(0), wholeValue(0) {}
};

// Helper functions
std::string tokenKindToString(TokenKind kind);
TokenKind getTokenCategory(TokenKind token);
int getTokenPrecedence(TokenKind token);
bool hasHigherPriority(TokenKind token1, TokenKind token2);