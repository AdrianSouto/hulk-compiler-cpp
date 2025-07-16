#include "Lexer/DFA.hpp"

int DFA::getNextState(char c, int currentState) {
    if (!isValidChar(c)) return -2; // Invalid character
    
    auto it = transitions.find(currentState);
    if (it == transitions.end()) return -1; // No transitions from this state
    
    const auto& stateTransitions = it->second;
    for (const auto& trans : stateTransitions) {
        if (trans.character == c || trans.type == "any") {
            return trans.toStates[0]; // DFA has only one destination per transition
        }
    }
    
    return -1; // No transition for this character
}

bool DFA::isValidChar(char c) const {
    return alphabet.find(c) != alphabet.end();
}

bool DFA::isFinalState(int state) const {
    for (int finalState : finalStates) {
        if (state == finalState) return true;
    }
    return false;
}

bool DFA::evaluate(const std::string& input) {
    int currentState = startState;
    
    for (char c : input) {
        currentState = getNextState(c, currentState);
        if (currentState < 0) return false;
    }
    
    return isFinalState(currentState);
}

TokenType DFA::getTokenType(int state) const {
    auto it = finalStateTokenTypes.find(state);
    if (it != finalStateTokenTypes.end()) {
        return it->second;
    }
    return TOKEN_UNKNOWN;
}

void DFA::setTokenType(int state, TokenType type) {
    finalStateTokenTypes[state] = type;
}