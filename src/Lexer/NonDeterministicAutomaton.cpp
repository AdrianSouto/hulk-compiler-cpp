#include "Lexer/NonDeterministicAutomaton.hpp"
#include <algorithm>
#include <queue>
#include <map>

bool NonDeterministicAutomaton::evaluate(const std::string& input) {
    std::set<int> currentStates;
    auto [initialClosure, _] = computeEpsilonClosure(initialState, true);
    currentStates = initialClosure;
    
    for (char ch : input) {
        auto [nextStates, __] = computeMove(currentStates, ch);
        auto [closureStates, ___] = computeEpsilonClosureSet(nextStates);
        currentStates = closureStates;
        
        if (currentStates.empty()) {
            return false;
        }
    }
    

    for (int state : currentStates) {
        if (isAcceptingState(state)) {
            return true;
        }
    }
    
    return false;
}

bool NonDeterministicAutomaton::isValidInput(char input) const {
    return inputAlphabet.find(input) != inputAlphabet.end() || 
           inputAlphabet.find('~') != inputAlphabet.end();
}

std::pair<std::vector<int>, bool> NonDeterministicAutomaton::getNextStates(char input, int state, bool useWildcard) const {
    std::vector<int> nextStates;
    bool found = false;
    
    auto it = stateTransitions.find(state);
    if (it != stateTransitions.end()) {
        for (const auto& transition : it->second) {
            if (transition.transitionType == "standard" && transition.inputChar == input) {
                nextStates.insert(nextStates.end(), transition.destinationStates.begin(), transition.destinationStates.end());
                found = true;
            } else if (useWildcard && transition.transitionType == "wildcard") {
                nextStates.insert(nextStates.end(), transition.destinationStates.begin(), transition.destinationStates.end());
                found = true;
            }
        }
    }
    
    return {nextStates, found};
}

bool NonDeterministicAutomaton::isAcceptingState(int state) const {
    return std::find(acceptingStates.begin(), acceptingStates.end(), state) != acceptingStates.end();
}

std::pair<std::set<int>, bool> NonDeterministicAutomaton::computeEpsilonClosure(int state, bool /* isInitial */) {
    std::set<int> closure;
    std::queue<int> workQueue;
    
    closure.insert(state);
    workQueue.push(state);
    
    while (!workQueue.empty()) {
        int currentState = workQueue.front();
        workQueue.pop();
        
        auto [epsilonStates, found] = getEpsilonTransitions(currentState);
        for (int epsilonState : epsilonStates) {
            if (closure.find(epsilonState) == closure.end()) {
                closure.insert(epsilonState);
                workQueue.push(epsilonState);
            }
        }
    }
    
    return {closure, !closure.empty()};
}

std::pair<std::set<int>, bool> NonDeterministicAutomaton::computeEpsilonClosureSet(const std::set<int>& states) {
    std::set<int> totalClosure;
    
    for (int state : states) {
        auto [closure, found] = computeEpsilonClosure(state, false);
        totalClosure.insert(closure.begin(), closure.end());
    }
    
    return {totalClosure, !totalClosure.empty()};
}

std::pair<std::set<int>, bool> NonDeterministicAutomaton::computeMove(const std::set<int>& states, char input) {
    std::set<int> moveStates;
    
    for (int state : states) {
        auto [nextStates, found] = getNextStates(input, state, true);
        moveStates.insert(nextStates.begin(), nextStates.end());
    }
    
    return {moveStates, !moveStates.empty()};
}

std::pair<std::vector<int>, bool> NonDeterministicAutomaton::getEpsilonTransitions(int state) const {
    std::vector<int> epsilonStates;
    
    auto it = stateTransitions.find(state);
    if (it != stateTransitions.end()) {
        for (const auto& transition : it->second) {
            if (transition.transitionType == "epsilon") {
                epsilonStates.insert(epsilonStates.end(), transition.destinationStates.begin(), transition.destinationStates.end());
            }
        }
    }
    
    return {epsilonStates, !epsilonStates.empty()};
}

FiniteAutomaton NonDeterministicAutomaton::convertToDeterministic() {
    std::map<std::set<int>, int> stateSetToId;
    std::map<int, std::vector<StateTransition>> dfaTransitions;
    std::vector<int> dfaAcceptingStates;
    std::map<int, TokenKind> dfaAcceptingStateTokens;
    std::queue<std::set<int>> workQueue;
    
    int nextStateId = 0;
    

    auto [initialClosure, _] = computeEpsilonClosure(initialState, true);
    stateSetToId[initialClosure] = nextStateId++;
    workQueue.push(initialClosure);
    
    while (!workQueue.empty()) {
        std::set<int> currentStateSet = workQueue.front();
        workQueue.pop();
        
        int currentDfaState = stateSetToId[currentStateSet];
        

        for (int nfaState : currentStateSet) {
            if (isAcceptingState(nfaState)) {
                dfaAcceptingStates.push_back(currentDfaState);
                auto it = acceptingStateTokens.find(nfaState);
                if (it != acceptingStateTokens.end()) {
                    dfaAcceptingStateTokens[currentDfaState] = it->second;
                }
                break;
            }
        }
        

        for (char ch : inputAlphabet) {
            if (ch == '$' || ch == '~') continue;
            
            auto [moveStates, found] = computeMove(currentStateSet, ch);
            if (!found || moveStates.empty()) continue;
            
            auto [closureStates, __] = computeEpsilonClosureSet(moveStates);
            
            if (!closureStates.empty()) {
                int targetState;
                if (stateSetToId.find(closureStates) == stateSetToId.end()) {
                    targetState = nextStateId++;
                    stateSetToId[closureStates] = targetState;
                    workQueue.push(closureStates);
                } else {
                    targetState = stateSetToId[closureStates];
                }
                
                dfaTransitions[currentDfaState].push_back(
                    StateTransition(ch, {targetState}, "standard")
                );
            }
        }
    }
    
    FiniteAutomaton dfa(0, nextStateId, dfaAcceptingStates, inputAlphabet, dfaTransitions);
    

    for (const auto& [state, kind] : dfaAcceptingStateTokens) {
        dfa.setTokenKind(state, kind);
    }
    
    return dfa;
}


NonDeterministicAutomaton NonDeterministicAutomaton::createEmpty() {
    return NonDeterministicAutomaton(0, 1, {}, {}, {});
}

NonDeterministicAutomaton NonDeterministicAutomaton::createEpsilon() {
    std::map<int, std::vector<StateTransition>> transitions;
    transitions[0].push_back(StateTransition('$', {1}, "epsilon"));
    return NonDeterministicAutomaton(0, 2, {1}, {'$'}, transitions);
}

NonDeterministicAutomaton NonDeterministicAutomaton::createWildcard() {
    std::map<int, std::vector<StateTransition>> transitions;
    transitions[0].push_back(StateTransition('~', {1}, "wildcard"));
    return NonDeterministicAutomaton(0, 2, {1}, {'~'}, transitions);
}

NonDeterministicAutomaton NonDeterministicAutomaton::createCharacter(char character) {
    std::map<int, std::vector<StateTransition>> transitions;
    transitions[0].push_back(StateTransition(character, {1}, "standard"));
    return NonDeterministicAutomaton(0, 2, {1}, {character}, transitions);
}

NonDeterministicAutomaton NonDeterministicAutomaton::createCharacterRange(char from, char to) {
    std::set<char> alphabet;
    std::map<int, std::vector<StateTransition>> transitions;
    
    for (char ch = from; ch <= to; ch++) {
        alphabet.insert(ch);
        transitions[0].push_back(StateTransition(ch, {1}, "standard"));
    }
    
    return NonDeterministicAutomaton(0, 2, {1}, alphabet, transitions);
}

NonDeterministicAutomaton NonDeterministicAutomaton::createUnion(const NonDeterministicAutomaton& first, const NonDeterministicAutomaton& second) {
    int offset1 = 1;
    int offset2 = offset1 + first.stateCount;
    int newStateCount = offset2 + second.stateCount;
    
    std::set<char> combinedAlphabet = first.inputAlphabet;
    combinedAlphabet.insert(second.inputAlphabet.begin(), second.inputAlphabet.end());
    
    std::map<int, std::vector<StateTransition>> newTransitions;
    

    newTransitions[0].push_back(StateTransition('$', {offset1 + first.initialState}, "epsilon"));
    newTransitions[0].push_back(StateTransition('$', {offset2 + second.initialState}, "epsilon"));
    

    for (const auto& [state, transitions] : first.stateTransitions) {
        for (const auto& transition : transitions) {
            std::vector<int> newDestinations;
            for (int dest : transition.destinationStates) {
                newDestinations.push_back(dest + offset1);
            }
            newTransitions[state + offset1].push_back(
                StateTransition(transition.inputChar, newDestinations, transition.transitionType)
            );
        }
    }
    

    for (const auto& [state, transitions] : second.stateTransitions) {
        for (const auto& transition : transitions) {
            std::vector<int> newDestinations;
            for (int dest : transition.destinationStates) {
                newDestinations.push_back(dest + offset2);
            }
            newTransitions[state + offset2].push_back(
                StateTransition(transition.inputChar, newDestinations, transition.transitionType)
            );
        }
    }
    

    std::vector<int> newAcceptingStates;
    for (int state : first.acceptingStates) {
        newAcceptingStates.push_back(state + offset1);
    }
    for (int state : second.acceptingStates) {
        newAcceptingStates.push_back(state + offset2);
    }
    

    NonDeterministicAutomaton result(0, newStateCount, newAcceptingStates, combinedAlphabet, newTransitions);
    

    for (const auto& [state, tokenKind] : first.acceptingStateTokens) {
        result.setTokenKind(state + offset1, tokenKind);
    }
    

    for (const auto& [state, tokenKind] : second.acceptingStateTokens) {
        result.setTokenKind(state + offset2, tokenKind);
    }
    
    return result;
}

NonDeterministicAutomaton NonDeterministicAutomaton::createConcatenation(const NonDeterministicAutomaton& first, const NonDeterministicAutomaton& second) {
    int offset = first.stateCount;
    int newStateCount = first.stateCount + second.stateCount;
    
    std::set<char> combinedAlphabet = first.inputAlphabet;
    combinedAlphabet.insert(second.inputAlphabet.begin(), second.inputAlphabet.end());
    
    std::map<int, std::vector<StateTransition>> newTransitions = first.stateTransitions;
    

    for (int acceptingState : first.acceptingStates) {
        newTransitions[acceptingState].push_back(
            StateTransition('$', {offset + second.initialState}, "epsilon")
        );
    }
    

    for (const auto& [state, transitions] : second.stateTransitions) {
        for (const auto& transition : transitions) {
            std::vector<int> newDestinations;
            for (int dest : transition.destinationStates) {
                newDestinations.push_back(dest + offset);
            }
            newTransitions[state + offset].push_back(
                StateTransition(transition.inputChar, newDestinations, transition.transitionType)
            );
        }
    }
    

    std::vector<int> newAcceptingStates;
    for (int state : second.acceptingStates) {
        newAcceptingStates.push_back(state + offset);
    }
    

    NonDeterministicAutomaton result(first.initialState, newStateCount, newAcceptingStates, combinedAlphabet, newTransitions);
    

    for (const auto& [state, tokenKind] : second.acceptingStateTokens) {
        result.setTokenKind(state + offset, tokenKind);
    }
    
    return result;
}

NonDeterministicAutomaton NonDeterministicAutomaton::createKleeneStar(const NonDeterministicAutomaton& automaton) {
    int offset = 1;
    int newStateCount = automaton.stateCount + 2;
    
    std::map<int, std::vector<StateTransition>> newTransitions;
    

    newTransitions[0].push_back(StateTransition('$', {offset + automaton.initialState}, "epsilon"));
    newTransitions[0].push_back(StateTransition('$', {newStateCount - 1}, "epsilon"));
    

    for (const auto& [state, transitions] : automaton.stateTransitions) {
        for (const auto& transition : transitions) {
            std::vector<int> newDestinations;
            for (int dest : transition.destinationStates) {
                newDestinations.push_back(dest + offset);
            }
            newTransitions[state + offset].push_back(
                StateTransition(transition.inputChar, newDestinations, transition.transitionType)
            );
        }
    }
    

    for (int acceptingState : automaton.acceptingStates) {
        newTransitions[acceptingState + offset].push_back(
            StateTransition('$', {offset + automaton.initialState}, "epsilon")
        );
        newTransitions[acceptingState + offset].push_back(
            StateTransition('$', {newStateCount - 1}, "epsilon")
        );
    }
    
    return NonDeterministicAutomaton(0, newStateCount, {newStateCount - 1}, automaton.inputAlphabet, newTransitions);
}