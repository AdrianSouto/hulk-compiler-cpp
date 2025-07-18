#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
#include <queue>
#include "FiniteAutomaton.hpp"
#include "Token.hpp"
#include "StateTransition.hpp"

class NonDeterministicAutomaton {
private:
    int initialState;
    int stateCount;
    std::set<char> inputAlphabet;
    std::map<int, std::vector<StateTransition>> stateTransitions;
    std::vector<int> acceptingStates;
    std::map<int, TokenKind> acceptingStateTokens;
    
    // Helper methods for deterministic conversion
    std::pair<std::set<int>, bool> computeEpsilonClosure(int state, bool isInitial);
    std::pair<std::set<int>, bool> computeEpsilonClosureSet(const std::set<int>& states);
    std::pair<std::set<int>, bool> computeMove(const std::set<int>& states, char input);
    std::pair<std::vector<int>, bool> getEpsilonTransitions(int state) const;

public:
    // Constructor
    NonDeterministicAutomaton(int initial, int count, const std::vector<int>& accepting,
        const std::set<char>& alphabet, const std::map<int, std::vector<StateTransition>>& transitions)
        : initialState(initial), stateCount(count), inputAlphabet(alphabet),
          stateTransitions(transitions), acceptingStates(accepting) {}
    
    // Evaluate a string
    bool evaluate(const std::string& input);
    
    // Check if character is valid
    bool isValidInput(char input) const;
    
    // Get next states given current state and input
    std::pair<std::vector<int>, bool> getNextStates(char input, int state, bool useWildcard) const;
    
    // Check if state is accepting
    bool isAcceptingState(int state) const;
    
    // Convert to deterministic automaton
    FiniteAutomaton convertToDeterministic();
    
    // Factory methods for creating automatons from regular expressions
    static NonDeterministicAutomaton createEmpty();
    static NonDeterministicAutomaton createEpsilon();
    static NonDeterministicAutomaton createWildcard();
    static NonDeterministicAutomaton createCharacter(char character);
    static NonDeterministicAutomaton createCharacterRange(char from, char to);
    static NonDeterministicAutomaton createUnion(const NonDeterministicAutomaton& first, const NonDeterministicAutomaton& second);
    static NonDeterministicAutomaton createConcatenation(const NonDeterministicAutomaton& first, const NonDeterministicAutomaton& second);
    static NonDeterministicAutomaton createKleeneStar(const NonDeterministicAutomaton& automaton);
    
    // Getters
    int getStateCount() const { return stateCount; }
    const std::vector<int>& getAcceptingStates() const { return acceptingStates; }
    void setTokenKind(int state, TokenKind kind) { acceptingStateTokens[state] = kind; }
};