#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
#include "Token.hpp"
#include "Transition.hpp"

class DFA {
private:
    int startState;
    int totalStates;
    std::vector<int> finalStates;
    std::set<char> alphabet;
    std::map<int, std::vector<Transition>> transitions;
    std::map<int, TokenType> finalStateTokenTypes;

public:
    // Constructor
    DFA(int start, int total, const std::vector<int>& finals,
        const std::set<char>& alpha, const std::map<int, std::vector<Transition>>& trans)
        : startState(start), totalStates(total), finalStates(finals),
          alphabet(alpha), transitions(trans) {}
    
    // Default constructor
    DFA() : startState(0), totalStates(0) {}
    
    // Get next state given current state and input character
    int getNextState(char c, int currentState);
    
    // Check if a character is valid in the alphabet
    bool isValidChar(char c) const;
    
    // Check if a state is final
    bool isFinalState(int state) const;
    
    // Evaluate a string
    bool evaluate(const std::string& input);
    
    // Getters
    int getStartState() const { return startState; }
    TokenType getTokenType(int state) const;
    void setTokenType(int state, TokenType type);
    
    // Friend class for NFA to access private members
    friend class NFA;
};