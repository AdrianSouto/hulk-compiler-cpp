#pragma once
#include <string>
#include <memory>
#include "PatternExpression.hpp"

class PatternParser {
private:
    std::string pattern;
    size_t position;
    
    // Parsing methods
    PatternPtr parseExpression();
    PatternPtr parseTerm();
    PatternPtr parseFactor();
    PatternPtr parseAtom();
    PatternPtr parseCharacterClass();
    PatternPtr parseEscape();
    
    // Helper methods
    char peek();
    char advance();
    bool isAtEnd();
    bool match(char expected);
    
public:
    PatternParser(const std::string& pat) : pattern(pat), position(0) {}
    
    PatternPtr parse();
};