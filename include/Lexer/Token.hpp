#pragma once
#include <string>

enum TokenType {
    // Literals and identifiers
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    
    // Keywords
    TOKEN_LET,
    TOKEN_IN,
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
    TOKEN_IS,
    TOKEN_AS,
    TOKEN_PRINT,
    TOKEN_TRUE,
    TOKEN_FALSE,
    
    // Type keywords
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_BOOLEAN,
    
    // Multi-character operators
    TOKEN_EQEQ,         // ==
    TOKEN_NOTEQ,        // !=
    TOKEN_LESSEQ,       // <=
    TOKEN_GREATEREQ,    // >=
    TOKEN_AND,          // &&
    TOKEN_OR,           // |
    TOKEN_CONCAT_SPACE, // @@
    TOKEN_ARROW,        // =>
    TOKEN_POWER,        // **
    TOKEN_ASSIGN,       // :=
    
    // Single character operators
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_MODULO,       // %
    TOKEN_CONCAT,       // @
    TOKEN_EQUALS,       // =
    TOKEN_LESS,         // <
    TOKEN_GREATER,      // >
    TOKEN_NOT,          // !
    
    // Delimiters
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_SEMICOLON,    // ;
    TOKEN_COMMA,        // ,
    TOKEN_COLON,        // :
    TOKEN_DOT,          // .
    
    // Special tokens
    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_UNKNOWN
};

struct Token {
    std::string lexeme;
    TokenType type;
    int line;
    int column;
    
    // For numeric values
    union {
        int intValue;
        double doubleValue;
    };
    
    Token(const std::string& lex, TokenType t, int l, int c) 
        : lexeme(lex), type(t), line(l), column(c), intValue(0) {}
    
    Token() : type(TOKEN_UNKNOWN), line(0), column(0), intValue(0) {}
};

// Helper functions
std::string tokenTypeToString(TokenType type);
TokenType getTokenCategory(TokenType token);
int getTokenPrecedence(TokenType token);
bool hasHigherPriority(TokenType token1, TokenType token2);