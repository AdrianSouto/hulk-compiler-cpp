#ifndef FLEX_COMPATIBLE_LEXER_HPP
#define FLEX_COMPATIBLE_LEXER_HPP

#include "../AllHeaders.hpp"
#include <string>
#include <fstream>
#include <iostream>


int yylex();
extern char* yytext;
extern int yyleng;
extern FILE* yyin;


union YYSTYPE;
extern YYSTYPE yylval;

class FlexCompatibleLexer {
private:
    static std::string input_buffer;
    static size_t current_pos;
    static size_t buffer_size;
    static std::string current_token;
    
    static void ensureBufferLoaded();
    static char peek(size_t offset = 0);
    static char advance();
    static void skipWhitespace();
    static void skipSingleLineComment();
    static void skipMultiLineComment();
    static bool isAtEnd();
    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isAlphaNumeric(char c);
    static std::string readNumber();
    static std::string readString();
    static std::string readIdentifier();
    static int checkKeyword(const std::string& text);
    static std::string processEscapeSequences(const std::string& str);
    
public:
    static void setInput(const std::string& input);
    static void setInputFile(const std::string& filename);
    static int getNextToken();
    static void reset();
};

#endif