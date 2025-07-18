#pragma once
#include <string>

enum TokenType {
    // Keywords
    TOKEN_FUNCTION,
    TOKEN_TYPE,
    TOKEN_INHERITS,
    TOKEN_NEW,
    TOKEN_BASE,
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_IS,
    TOKEN_AS,
    TOKEN_LET,
    TOKEN_PRINT,
    TOKEN_TRUE,
    TOKEN_FALSE,
    
    // Type keywords
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_BOOLEAN,
    
    // Literals and identifiers
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    
    // Delimiters
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_SEMICOLON,     // ;
    TOKEN_COMMA,         // ,
    TOKEN_COLON,         // :
    TOKEN_DOT,           // .
    
    // Operators
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_MULTIPLY,      // *
    TOKEN_DIVIDE,        // /
    TOKEN_MODULO,        // %
    TOKEN_POWER,         // **
    TOKEN_CONCAT,        // @
    TOKEN_CONCAT_SPACE,  // @@
    TOKEN_EQUALS,        // =
    TOKEN_ASSIGN,        // :=
    TOKEN_EQEQ,          // ==
    TOKEN_NOTEQ,         // !=
    TOKEN_LESS,          // <
    TOKEN_LESSEQ,        // <=
    TOKEN_GREATER,       // >
    TOKEN_GREATEREQ,     // >=
    TOKEN_AND,           // &&
    TOKEN_OR,            // |
    TOKEN_NOT,           // !
    TOKEN_ARROW,         // =>
    
    // Special tokens
    TOKEN_EOF,
    TOKEN_INVALID,
    TOKEN_UNDEFINED
};

struct Token {
    std::string text;
    TokenType type;
    int row;
    int col;
    
    // For numeric values
    union {
        int wholeValue;
        double realValue;
    };
    
    Token(const std::string& txt, TokenType t, int r, int c) 
        : text(txt), type(t), row(r), col(c), wholeValue(0) {}
    
    Token() : type(TOKEN_UNDEFINED), row(0), col(0), wholeValue(0) {}
};

// Helper functions
std::string tokenTypeToString(TokenType type);
int getTokenPrecedence(TokenType token);
bool hasHigherPriority(TokenType token1, TokenType token2);