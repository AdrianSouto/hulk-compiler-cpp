#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include "Lexer/Lexer.hpp"

// Include necessary headers for parser.hpp
#include "AllHeaders.hpp"

// Now include the generated parser header
#include "../../hulk/parser.hpp"

// Global variables to maintain compatibility with flex
extern YYLTYPE yylloc;
extern YYSTYPE yylval;
int yylineno = 1;
int yycolumn = 1;

// yytext for compatibility
char* yytext = nullptr;

// Implement yylex without extern "C" since we're defining it here
int yylex();

// Static lexer instance
static Lexer* lexerInstance = nullptr;
static std::vector<Token> currentTokens;
static size_t currentTokenIndex = 0;
static std::string inputBuffer;

// Initialize the lexer
void initializeLexer() {
    if (!lexerInstance) {
        lexerInstance = new Lexer();
    }
}

// Function to set input from file (compatible with flex interface)
void set_input_from_file(FILE* file) {
    initializeLexer();
    
    // Read entire file into string
    std::ostringstream ss;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), file)) {
        ss << buffer;
    }
    
    inputBuffer = ss.str();
    lexerInstance->reset();
    currentTokens = lexerInstance->tokenize(inputBuffer);
    currentTokenIndex = 0;
    yylineno = 1;
    yycolumn = 1;
}

// Function to set input from string (compatible with flex interface)
void set_input(const std::string& text) {
    initializeLexer();
    
    inputBuffer = text;
    lexerInstance->reset();
    currentTokens = lexerInstance->tokenize(inputBuffer);
    currentTokenIndex = 0;
    yylineno = 1;
    yycolumn = 1;
}

// Convert our token type to parser token type
int convertTokenType(TokenType type) {
    switch (type) {
        case TOKEN_LET: return LET;
        case TOKEN_IN: return IN;
        case TOKEN_FUNCTION: return FUNCTION;
        case TOKEN_TYPE: return TYPE;
        case TOKEN_INHERITS: return INHERITS;
        case TOKEN_NEW: return NEW;
        case TOKEN_BASE: return BASE;
        case TOKEN_IF: return IF;
        case TOKEN_ELIF: return ELIF;
        case TOKEN_ELSE: return ELSE;
        case TOKEN_WHILE: return WHILE;
        case TOKEN_FOR: return FOR;
        case TOKEN_IS: return IS;
        case TOKEN_AS: return AS;
        case TOKEN_PRINT: return PRINT;
        case TOKEN_TRUE: return TRUE;
        case TOKEN_FALSE: return FALSE;
        
        case TOKEN_TYPE_NUMBER: return TYPE_NUMBER;
        case TOKEN_TYPE_STRING: return TYPE_STRING;
        case TOKEN_TYPE_BOOLEAN: return TYPE_BOOLEAN;
        
        case TOKEN_EQEQ: return EQEQ;
        case TOKEN_NOTEQ: return NOTEQ;
        case TOKEN_LESSEQ: return LESSEQ;
        case TOKEN_GREATEREQ: return GREATEREQ;
        case TOKEN_AND: return AND;
        case TOKEN_OR: return OR;
        case TOKEN_CONCAT_SPACE: return CONCAT_SPACE;
        case TOKEN_ARROW: return ARROW;
        case TOKEN_POWER: return POWER;
        case TOKEN_ASSIGN: return ASSIGN;
        
        case TOKEN_PLUS: return PLUS;
        case TOKEN_MINUS: return MINUS;
        case TOKEN_MULTIPLY: return MULTIPLY;
        case TOKEN_DIVIDE: return DIVIDE;
        case TOKEN_MODULO: return MODULO;
        case TOKEN_CONCAT: return CONCAT;
        case TOKEN_EQUALS: return EQUALS;
        case TOKEN_LESS: return LESS;
        case TOKEN_GREATER: return GREATER;
        case TOKEN_NOT: return NOT;
        
        case TOKEN_LPAREN: return LPAREN;
        case TOKEN_RPAREN: return RPAREN;
        case TOKEN_LBRACE: return LBRACE;
        case TOKEN_RBRACE: return RBRACE;
        case TOKEN_SEMICOLON: return SEMICOLON;
        case TOKEN_COMMA: return COMMA;
        case TOKEN_COLON: return COLON;
        case TOKEN_DOT: return DOT;
        
        case TOKEN_IDENTIFIER: return IDENTIFIER;
        case TOKEN_NUMBER: return NUMBER;
        case TOKEN_STRING: return STRING;
        
        case TOKEN_EOF: return 0; // EOF in bison
        default: return -1;
    }
}

// Main lexer function (compatible with flex yylex)
int yylex() {
    if (currentTokenIndex >= currentTokens.size()) {
        return 0; // EOF
    }
    
    const Token& token = currentTokens[currentTokenIndex++];
    
    // Update location information
    yylloc.first_line = yylloc.last_line = token.line;
    yylloc.first_column = token.column;
    yylloc.last_column = token.column + token.lexeme.length() - 1;
    yylineno = token.line;
    
    // Set semantic value based on token type
    switch (token.type) {
        case TOKEN_IDENTIFIER:
            yylval.string = strdup(token.lexeme.c_str());
            break;
            
        case TOKEN_NUMBER: {
            // Check if it's a decimal or integer
            if (token.lexeme.find('.') != std::string::npos) {
                yylval.decimal = std::stod(token.lexeme);
            } else {
                yylval.number = std::stoi(token.lexeme);
            }
            break;
        }
        
        case TOKEN_STRING: {
            // Remove quotes and handle escape sequences
            std::string str = token.lexeme.substr(1, token.lexeme.length() - 2);
            std::string result;
            
            for (size_t i = 0; i < str.length(); ++i) {
                if (str[i] == '\\' && i + 1 < str.length()) {
                    switch (str[i + 1]) {
                        case 'n': result += '\n'; i++; break;
                        case 't': result += '\t'; i++; break;
                        case 'r': result += '\r'; i++; break;
                        case '\\': result += '\\'; i++; break;
                        case '"': result += '"'; i++; break;
                        default: result += str[i]; break;
                    }
                } else {
                    result += str[i];
                }
            }
            
            yylval.string = strdup(result.c_str());
            break;
        }
        
        default:
            break;
    }
    
    // Debug output (matching flex behavior)
    std::string tokenName = tokenTypeToString(token.type);
    if (token.type == TOKEN_IDENTIFIER) {
        printf("<ID:%s>\n", token.lexeme.c_str());
    } else if (token.type == TOKEN_NUMBER) {
        printf("<NUMBER:%s>\n", token.lexeme.c_str());
    } else if (token.type == TOKEN_STRING) {
        printf("<STRING:%s>\n", yylval.string);
    } else {
        printf("<%s>\n", tokenName.c_str());
    }
    
    return convertTokenType(token.type);
}

// Cleanup function
void cleanupLexer() {
    if (lexerInstance) {
        delete lexerInstance;
        lexerInstance = nullptr;
    }
    currentTokens.clear();
    currentTokenIndex = 0;
    inputBuffer.clear();
}