#pragma once
#include <string>
#include <vector>
#include <tuple>
#include "Token.hpp"
#include "DFA.hpp"

class Lexer {
private:
    DFA automaton;
    int currentLine;
    int currentColumn;
    
    // Initialize the DFA with all token patterns
    void initializePatterns();
    
    // Scan a single token starting at the given position
    Token scanToken(const std::string& input, size_t& position);
    
public:
    Lexer();
    
    // Tokenize the entire input
    std::vector<Token> tokenize(const std::string& input);
    
    // Reset lexer state
    void reset();
};