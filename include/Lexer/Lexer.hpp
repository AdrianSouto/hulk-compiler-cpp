#pragma once
#include <string>
#include <vector>
#include <tuple>
#include "Token.hpp"
#include "FiniteAutomaton.hpp"

class Lexer {
private:
    FiniteAutomaton recognizer;
    int currentRow;
    int currentCol;
    
    // Initialize the automaton with all token patterns
    void initializeRecognitionPatterns();
    
    // Scan a single token starting at the given position
    Token scanToken(const std::string& input, size_t& position);
    
public:
    Lexer();
    
    // Tokenize the entire input
    std::vector<Token> analyze(const std::string& input);
    
    // Reset lexer state
    void reset();
};