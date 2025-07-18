#include "Lexer/FiniteAutomaton.hpp"
#include <algorithm>

int FiniteAutomaton::getNextState(char input, int currentState) {
    auto it = stateTransitions.find(currentState);
    if (it == stateTransitions.end()) {
        return -1;
    }
    
    for (const auto& transition : it->second) {
        if (transition.transitionType == "standard" && transition.inputChar == input) {
            return transition.destinationStates.empty() ? -1 : transition.destinationStates[0];
        } else if (transition.transitionType == "wildcard" && isValidInput(input)) {
            return transition.destinationStates.empty() ? -1 : transition.destinationStates[0];
        }
    }
    
    return -1;
}

bool FiniteAutomaton::isValidInput(char input) const {
    return inputAlphabet.find(input) != inputAlphabet.end() || 
           inputAlphabet.find('~') != inputAlphabet.end();
}

bool FiniteAutomaton::isAcceptingState(int state) const {
    return std::find(acceptingStates.begin(), acceptingStates.end(), state) != acceptingStates.end();
}

bool FiniteAutomaton::evaluate(const std::string& input) {
    int currentState = initialState;
    
    for (char ch : input) {
        currentState = getNextState(ch, currentState);
        if (currentState < 0) {
            return false;
        }
    }
    
    return isAcceptingState(currentState);
}

TokenType FiniteAutomaton::getTokenType(int state) const {
    auto it = acceptingStateTokens.find(state);
    return (it != acceptingStateTokens.end()) ? it->second : TOKEN_UNDEFINED;
}

void FiniteAutomaton::setTokenType(int state, TokenType type) {
    acceptingStateTokens[state] = type;
}