#include "Lexer/Token.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TOKEN_FUNCTION: return "FUNCTION";
        case TOKEN_TYPE: return "TYPE";
        case TOKEN_INHERITS: return "INHERITS";
        case TOKEN_NEW: return "NEW";
        case TOKEN_BASE: return "BASE";
        case TOKEN_IF: return "IF";
        case TOKEN_ELIF: return "ELIF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_IN: return "IN";
        case TOKEN_IS: return "IS";
        case TOKEN_AS: return "AS";
        case TOKEN_LET: return "LET";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_TYPE_NUMBER: return "TYPE_NUMBER";
        case TOKEN_TYPE_STRING: return "TYPE_STRING";
        case TOKEN_TYPE_BOOLEAN: return "TYPE_BOOLEAN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_COLON: return "COLON";
        case TOKEN_DOT: return "DOT";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_POWER: return "POWER";
        case TOKEN_CONCAT: return "CONCAT";
        case TOKEN_CONCAT_SPACE: return "CONCAT_SPACE";
        case TOKEN_EQUALS: return "EQUALS";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_EQEQ: return "EQEQ";
        case TOKEN_NOTEQ: return "NOTEQ";
        case TOKEN_LESS: return "LESS";
        case TOKEN_LESSEQ: return "LESSEQ";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_GREATEREQ: return "GREATEREQ";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_ARROW: return "ARROW";
        case TOKEN_EOF: return "EOF";
        case TOKEN_INVALID: return "INVALID";
        case TOKEN_UNDEFINED: return "UNDEFINED";
        default: return "UNKNOWN";
    }
}

int getTokenPrecedence(TokenType token) {
    switch (token) {
        case TOKEN_OR: return 1;
        case TOKEN_AND: return 2;
        case TOKEN_NOT: return 3;
        case TOKEN_EQEQ:
        case TOKEN_NOTEQ:
        case TOKEN_LESS:
        case TOKEN_LESSEQ:
        case TOKEN_GREATER:
        case TOKEN_GREATEREQ: return 4;
        case TOKEN_CONCAT:
        case TOKEN_CONCAT_SPACE: return 5;
        case TOKEN_PLUS:
        case TOKEN_MINUS: return 6;
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
        case TOKEN_MODULO: return 7;
        case TOKEN_POWER: return 8;
        default: return 0;
    }
}

bool hasHigherPriority(TokenType token1, TokenType token2) {
    return getTokenPrecedence(token1) > getTokenPrecedence(token2);
}