#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
#include "Token.hpp"
#include "StateTransition.hpp"

class FiniteAutomaton {
private:
    int initialState;
    int stateCount;
    std::vector<int> acceptingStates;
    std::set<char> inputAlphabet;
    std::map<int, std::vector<StateTransition>> stateTransitions;
    std::map<int, TokenType> acceptingStateTokens;

public:
    // Constructor
    FiniteAutomaton(int initial, int count, const std::vector<int>& accepting,
        const std::set<char>& alphabet, const std::map<int, std::vector<StateTransition>>& transitions)
        : initialState(initial), stateCount(count), acceptingStates(accepting),
          inputAlphabet(alphabet), stateTransitions(transitions) {}
    
    // Default constructor
    FiniteAutomaton() : initialState(0), stateCount(0) {}
    
    // Get next state given current state and input character
    int getNextState(char input, int currentState);
    
    // Check if a character is valid in the alphabet
    bool isValidInput(char input) const;
    
    // Check if a state is accepting
    bool isAcceptingState(int state) const;
    
    // Evaluate a string
    bool evaluate(const std::string& input);
    
    // Getters
    int getInitialState() const { return initialState; }
    TokenType getTokenType(int state) const;
    void setTokenType(int state, TokenType type);
    
    // Friend class for NonDeterministicAutomaton to access private members
    friend class NonDeterministicAutomaton;
};