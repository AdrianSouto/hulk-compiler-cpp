#include "Lexer/Token.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // Literals and identifiers
        case TOKEN_NUMBER:         return "NUMBER";
        case TOKEN_STRING:         return "STRING";
        case TOKEN_IDENTIFIER:     return "IDENTIFIER";
        
        // Keywords
        case TOKEN_LET:           return "LET";
        case TOKEN_IN:            return "IN";
        case TOKEN_FUNCTION:      return "FUNCTION";
        case TOKEN_TYPE:          return "TYPE";
        case TOKEN_INHERITS:      return "INHERITS";
        case TOKEN_NEW:           return "NEW";
        case TOKEN_BASE:          return "BASE";
        case TOKEN_IF:            return "IF";
        case TOKEN_ELIF:          return "ELIF";
        case TOKEN_ELSE:          return "ELSE";
        case TOKEN_WHILE:         return "WHILE";
        case TOKEN_FOR:           return "FOR";
        case TOKEN_IS:            return "IS";
        case TOKEN_AS:            return "AS";
        case TOKEN_PRINT:         return "PRINT";
        case TOKEN_TRUE:          return "TRUE";
        case TOKEN_FALSE:         return "FALSE";
        
        // Type keywords
        case TOKEN_TYPE_NUMBER:   return "TYPE_NUMBER";
        case TOKEN_TYPE_STRING:   return "TYPE_STRING";
        case TOKEN_TYPE_BOOLEAN:  return "TYPE_BOOLEAN";
        
        // Multi-character operators
        case TOKEN_EQEQ:          return "EQEQ";
        case TOKEN_NOTEQ:         return "NOTEQ";
        case TOKEN_LESSEQ:        return "LESSEQ";
        case TOKEN_GREATEREQ:     return "GREATEREQ";
        case TOKEN_AND:           return "AND";
        case TOKEN_OR:            return "OR";
        case TOKEN_CONCAT_SPACE:  return "CONCAT_SPACE";
        case TOKEN_ARROW:         return "ARROW";
        case TOKEN_POWER:         return "POWER";
        case TOKEN_ASSIGN:        return "ASSIGN";
        
        // Single character operators
        case TOKEN_PLUS:          return "PLUS";
        case TOKEN_MINUS:         return "MINUS";
        case TOKEN_MULTIPLY:      return "MULTIPLY";
        case TOKEN_DIVIDE:        return "DIVIDE";
        case TOKEN_MODULO:        return "MODULO";
        case TOKEN_CONCAT:        return "CONCAT";
        case TOKEN_EQUALS:        return "EQUALS";
        case TOKEN_LESS:          return "LESS";
        case TOKEN_GREATER:       return "GREATER";
        case TOKEN_NOT:           return "NOT";
        
        // Delimiters
        case TOKEN_LPAREN:        return "LPAREN";
        case TOKEN_RPAREN:        return "RPAREN";
        case TOKEN_LBRACE:        return "LBRACE";
        case TOKEN_RBRACE:        return "RBRACE";
        case TOKEN_SEMICOLON:     return "SEMICOLON";
        case TOKEN_COMMA:         return "COMMA";
        case TOKEN_COLON:         return "COLON";
        case TOKEN_DOT:           return "DOT";
        
        // Special tokens
        case TOKEN_EOF:           return "EOF";
        case TOKEN_ERROR:         return "ERROR";
        case TOKEN_UNKNOWN:       return "UNKNOWN";
        
        default:                  return "UNKNOWN";
    }
}

TokenType getTokenCategory(TokenType token) {
    // Keywords
    if (token >= TOKEN_LET && token <= TOKEN_TYPE_BOOLEAN) {
        return TOKEN_IDENTIFIER; // Keywords are a category of identifiers
    }
    
    // Operators
    if ((token >= TOKEN_EQEQ && token <= TOKEN_ASSIGN) || 
        (token >= TOKEN_PLUS && token <= TOKEN_NOT)) {
        return TOKEN_UNKNOWN; // Using UNKNOWN as operator category
    }
    
    // Delimiters
    if (token >= TOKEN_LPAREN && token <= TOKEN_DOT) {
        return TOKEN_UNKNOWN; // Using UNKNOWN as delimiter category
    }
    
    return token;
}

int getTokenPrecedence(TokenType token) {
    switch(token) {
        // Keywords have highest precedence
        case TOKEN_LET:
        case TOKEN_IN:
        case TOKEN_FUNCTION:
        case TOKEN_TYPE:
        case TOKEN_INHERITS:
        case TOKEN_NEW:
        case TOKEN_BASE:
        case TOKEN_IF:
        case TOKEN_ELIF:
        case TOKEN_ELSE:
        case TOKEN_WHILE:
        case TOKEN_FOR:
        case TOKEN_IS:
        case TOKEN_AS:
        case TOKEN_PRINT:
        case TOKEN_TRUE:
        case TOKEN_FALSE:
        case TOKEN_TYPE_NUMBER:
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_BOOLEAN:
            return 20;
            
        case TOKEN_IDENTIFIER:
            return 15;
            
        case TOKEN_DOT:
            return 10;
            
        case TOKEN_NUMBER:
            return 7;
            
        // Operators
        case TOKEN_EQEQ:
        case TOKEN_NOTEQ:
        case TOKEN_LESSEQ:
        case TOKEN_GREATEREQ:
        case TOKEN_AND:
        case TOKEN_OR:
        case TOKEN_CONCAT_SPACE:
        case TOKEN_ARROW:
        case TOKEN_POWER:
        case TOKEN_ASSIGN:
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
        case TOKEN_MODULO:
        case TOKEN_CONCAT:
        case TOKEN_EQUALS:
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_NOT:
            return 6;
            
        case TOKEN_UNKNOWN:
            return -2;
            
        default:
            return -1;
    }
}

bool hasHigherPriority(TokenType token1, TokenType token2) {
    return getTokenPrecedence(token1) > getTokenPrecedence(token2);
}