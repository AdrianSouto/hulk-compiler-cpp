#ifndef NATIVE_LEXER_HPP
#define NATIVE_LEXER_HPP

#include "Token.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

class NativeLexer {
private:
    std::string input;
    size_t position;
    size_t current_line;
    size_t current_column;
    std::unordered_map<std::string, TokenType> keywords;
    
    void initializeKeywords();
    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    Token readNumber();
    Token readString();
    Token readIdentifier();
    Token readOperator();
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    std::string processEscapeSequences(const std::string& str);
    
public:
    NativeLexer();
    explicit NativeLexer(const std::string& input);
    
    void setInput(const std::string& input);
    Token nextToken();
    std::vector<Token> tokenize();
    bool isAtEnd() const;
    
    
    int getCurrentLine() const { return static_cast<int>(current_line); }
    int getCurrentColumn() const { return static_cast<int>(current_column); }
};

#endif 