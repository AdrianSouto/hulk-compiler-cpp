#pragma once
#include <string>
#include <memory>
#include "RegularExpression.hpp"

class RegexParser {
private:
    const std::string input;
    size_t position;
    
    // Parsing methods following precedence hierarchy
    RegExPtr parseUnion();
    RegExPtr parseConcatenation();
    RegExPtr parseClosure();
    RegExPtr parseAtom();
    
    // Helper methods
    char peek() const;
    char get();
    bool consume(char c);
    
public:
    RegexParser(const std::string& pattern) : input(pattern), position(0) {}
    
    RegExPtr parse();
};