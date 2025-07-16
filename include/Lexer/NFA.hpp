#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
#include <queue>
#include "DFA.hpp"
#include "Token.hpp"
#include "Transition.hpp"

class NFA {
private:
    int startState;
    int totalStates;
    std::set<char> alphabet;
    std::map<int, std::vector<Transition>> transitions;
    std::vector<int> finalStates;
    std::map<int, TokenType> finalStateTokenTypes;
    
    // Helper methods for DFA conversion
    std::pair<std::set<int>, bool> epsilonClosure(int state, bool initial);
    std::pair<std::set<int>, bool> epsilonClosureSet(const std::set<int>& states);
    std::pair<std::set<int>, bool> move(const std::set<int>& states, char symbol);
    std::pair<std::vector<int>, bool> getEpsilonTransitions(int state) const;

public:
    // Constructor
    NFA(int start, int total, const std::vector<int>& finals,
        const std::set<char>& alpha, const std::map<int, std::vector<Transition>>& trans)
        : startState(start), totalStates(total), alphabet(alpha),
          transitions(trans), finalStates(finals) {}
    
    // Evaluate a string
    bool evaluate(const std::string& input);
    
    // Check if character is valid
    bool isValidChar(char c) const;
    
    // Get next states given current state and input
    std::pair<std::vector<int>, bool> getNextStates(char c, int state, bool useAny) const;
    
    // Check if state is final
    bool isFinalState(int state) const;
    
    // Convert to DFA
    DFA convertToDFA();
    
    // Factory methods for creating NFAs from regular expressions
    static NFA createEmpty();
    static NFA createEpsilon();
    static NFA createAny();
    static NFA createSymbol(char symbol);
    static NFA createRange(char from, char to);
    static NFA createUnion(const NFA& a1, const NFA& a2);
    static NFA createConcatenation(const NFA& a1, const NFA& a2);
    static NFA createClosure(const NFA& a1);
    
    // Getters
    int getTotalStates() const { return totalStates; }
    const std::vector<int>& getFinalStates() const { return finalStates; }
    void setTokenType(int state, TokenType type) { finalStateTokenTypes[state] = type; }
};