#include "Lexer/NFA.hpp"
#include <algorithm>
#include <iostream>

bool NFA::evaluate(const std::string& input) {
    std::set<int> currentStates = {startState};
    
    // Add epsilon closure of start state
    auto [startEpsClosure, _] = epsilonClosure(startState, false);
    currentStates.insert(startEpsClosure.begin(), startEpsClosure.end());
    currentStates.insert(startState);
    
    for (char symbol : input) {
        std::set<int> nextStates;
        
        // For each current state
        for (int state : currentStates) {
            // Get next states for this symbol
            auto [next, _2] = getNextStates(symbol, state, true);
            for (int nextState : next) {
                nextStates.insert(nextState);
                // Add epsilon closure of next state
                auto [epsClosure, _3] = epsilonClosure(nextState, false);
                nextStates.insert(epsClosure.begin(), epsClosure.end());
            }
        }
        
        currentStates = nextStates;
    }
    
    // Check if any current state is final
    for (int state : currentStates) {
        if (isFinalState(state)) return true;
    }
    
    return false;
}

bool NFA::isValidChar(char c) const {
    return alphabet.find(c) != alphabet.end();
}

std::pair<std::vector<int>, bool> NFA::getNextStates(char c, int state, bool useAny) const {
    auto it = transitions.find(state);
    if (it == transitions.end()) return {{}, false};
    
    std::vector<int> nextStates;
    bool foundAny = false;
    
    for (const auto& trans : it->second) {
        if (trans.character == c || (useAny && trans.type == "any")) {
            if (trans.type == "any") foundAny = true;
            nextStates.insert(nextStates.end(), trans.toStates.begin(), trans.toStates.end());
        }
    }
    
    return {nextStates, foundAny};
}

bool NFA::isFinalState(int state) const {
    return std::find(finalStates.begin(), finalStates.end(), state) != finalStates.end();
}

std::pair<std::set<int>, bool> NFA::epsilonClosure(int state, bool initial) {
    std::set<int> closure;
    std::queue<int> pending;
    pending.push(state);
    bool foundAny = false;
    
    while (!pending.empty()) {
        int current = pending.front();
        pending.pop();
        
        if (closure.find(current) != closure.end()) continue;
        
        auto [epsTransitions, useAny] = getEpsilonTransitions(current);
        if (!foundAny) foundAny = useAny;
        
        for (int nextState : epsTransitions) {
            pending.push(nextState);
        }
        
        if (!initial || current != state) {
            closure.insert(current);
        }
    }
    
    return {closure, foundAny};
}

std::pair<std::set<int>, bool> NFA::epsilonClosureSet(const std::set<int>& states) {
    std::set<int> closure = states; // Include initial states
    bool foundAny = false;
    
    for (int state : states) {
        auto [stateClosure, useAny] = epsilonClosure(state, false);
        if (!foundAny) foundAny = useAny;
        closure.insert(stateClosure.begin(), stateClosure.end());
    }
    
    return {closure, foundAny};
}

std::pair<std::set<int>, bool> NFA::move(const std::set<int>& states, char symbol) {
    std::set<int> result;
    bool useAny = false;
    
    for (int state : states) {
        auto [nextStates, foundAny] = getNextStates(symbol, state, true);
        if (!useAny) useAny = foundAny;
        result.insert(nextStates.begin(), nextStates.end());
    }
    
    return {result, useAny};
}

std::pair<std::vector<int>, bool> NFA::getEpsilonTransitions(int state) const {
    return getNextStates('$', state, false);
}

DFA NFA::convertToDFA() {
    std::map<std::set<int>, int> dfaStates;
    std::queue<std::set<int>> pending;
    std::map<int, std::vector<Transition>> dfaTransitions;
    int stateCounter = 0;
    
    // Start with epsilon closure of start state
    auto [startClosure, _] = epsilonClosureSet({startState});
    pending.push(startClosure);
    dfaStates[startClosure] = stateCounter++;
    
    while (!pending.empty()) {
        std::set<int> currentSet = pending.front();
        pending.pop();
        
        for (char symbol : alphabet) {
            if (symbol == '$' || symbol == '~') continue; // Skip epsilon and any
            
            auto [moveSet, useAny] = move(currentSet, symbol);
            auto [closure, useAnyE] = epsilonClosureSet(moveSet);
            
            if (dfaStates.find(closure) == dfaStates.end()) {
                if (closure.empty()) {
                    dfaStates[closure] = -1;
                } else {
                    dfaStates[closure] = stateCounter++;
                }
                pending.push(closure);
            }
            
            if (dfaTransitions.find(dfaStates[currentSet]) == dfaTransitions.end()) {
                dfaTransitions[dfaStates[currentSet]] = {};
            }
            
            if (!useAny) {
                dfaTransitions[dfaStates[currentSet]].push_back(
                    Transition(symbol, {dfaStates[closure]}, "normal")
                );
            } else {
                dfaTransitions[dfaStates[currentSet]].push_back(
                    Transition(symbol, {dfaStates[closure]}, "any")
                );
                break;
            }
        }
    }
    
    // Determine final states and their token types
    std::vector<int> dfaFinalStates;
    std::map<int, TokenType> dfaFinalTokenTypes;
    
    for (const auto& [stateSet, dfaStateId] : dfaStates) {
        TokenType token = TOKEN_UNKNOWN;
        
        for (int nfaState : stateSet) {
            if (isFinalState(nfaState)) {
                auto it = finalStateTokenTypes.find(nfaState);
                if (it != finalStateTokenTypes.end()) {
                    TokenType stateToken = it->second;
                    if (hasHigherPriority(stateToken, token)) {
                        token = stateToken;
                    }
                }
                dfaFinalStates.push_back(dfaStateId);
            }
        }
        
        if (token != TOKEN_UNKNOWN) {
            dfaFinalTokenTypes[dfaStateId] = token;
        }
    }
    
    // Create DFA
    DFA dfa(0, stateCounter, dfaFinalStates, alphabet, dfaTransitions);
    dfa.finalStateTokenTypes = dfaFinalTokenTypes;
    
    return dfa;
}

// Factory methods
NFA NFA::createEmpty() {
    return NFA(0, 0, {}, {}, {});
}

NFA NFA::createEpsilon() {
    std::map<int, std::vector<Transition>> trans;
    trans[0] = {Transition('$', {1}, "epsilon")};
    return NFA(0, 2, {1}, {'$'}, trans);
}

NFA NFA::createAny() {
    std::map<int, std::vector<Transition>> trans;
    trans[0] = {Transition('~', {1}, "any")};
    return NFA(0, 2, {1}, {'~'}, trans);
}

NFA NFA::createSymbol(char symbol) {
    std::map<int, std::vector<Transition>> trans;
    trans[0] = {Transition(symbol, {1}, "normal")};
    return NFA(0, 2, {1}, {symbol}, trans);
}

NFA NFA::createRange(char from, char to) {
    int start = 0;
    int end = to - from + 2;
    std::set<char> alpha;
    std::map<int, std::vector<Transition>> trans;
    
    int state = 1;
    for (char c = from; c <= to; ++c, ++state) {
        alpha.insert(c);
        trans[start].push_back(Transition(c, {state}, "normal"));
        trans[state].push_back(Transition('$', {end}, "epsilon"));
    }
    
    return NFA(start, to - from + 3, {end}, alpha, trans);
}

NFA NFA::createUnion(const NFA& a1, const NFA& a2) {
    int newStart = a1.totalStates + a2.totalStates;
    int newTotal = a1.totalStates + a2.totalStates + 2;
    
    std::vector<int> newFinals = a1.finalStates;
    std::set<char> newAlphabet = a1.alphabet;
    newAlphabet.insert(a2.alphabet.begin(), a2.alphabet.end());
    
    std::map<int, std::vector<Transition>> newTransitions = a1.transitions;
    
    // Add a2's transitions with shifted states
    for (const auto& [state, transitions] : a2.transitions) {
        int shiftedState = state + a1.totalStates;
        for (auto trans : transitions) {
            for (size_t i = 0; i < trans.toStates.size(); i++) {
                trans.toStates[i] += a1.totalStates;
            }
            newTransitions[shiftedState].push_back(trans);
        }
    }
    
    // Add epsilon transitions from new start
    newAlphabet.insert('$');
    newTransitions[newStart] = {
        Transition('$', {a1.startState, a2.startState + a1.totalStates}, "epsilon")
    };
    
    // Merge final state token types
    auto newFinalTokenTypes = a1.finalStateTokenTypes;
    for (const auto& [state, type] : a2.finalStateTokenTypes) {
        newFinalTokenTypes[state + a1.totalStates] = type;
    }
    
    // Add a2's final states
    for (int fs : a2.finalStates) {
        newFinals.push_back(fs + a1.totalStates);
    }
    
    NFA result(newStart, newTotal, newFinals, newAlphabet, newTransitions);
    result.finalStateTokenTypes = newFinalTokenTypes;
    return result;
}

NFA NFA::createConcatenation(const NFA& a1, const NFA& a2) {
    int newTotal = a1.totalStates + a2.totalStates;
    
    std::set<char> newAlphabet = a1.alphabet;
    newAlphabet.insert(a2.alphabet.begin(), a2.alphabet.end());
    
    std::vector<int> newFinals;
    for (int fs : a2.finalStates) {
        newFinals.push_back(fs + a1.totalStates);
    }
    
    std::map<int, std::vector<Transition>> newTransitions = a1.transitions;
    
    // Add a2's transitions with shifted states
    for (const auto& [state, transitions] : a2.transitions) {
        int shiftedState = state + a1.totalStates;
        for (auto trans : transitions) {
            for (size_t i = 0; i < trans.toStates.size(); i++) {
                trans.toStates[i] += a1.totalStates;
            }
            newTransitions[shiftedState].push_back(trans);
        }
    }
    
    // Connect a1's final states to a2's start state
    newAlphabet.insert('$');
    for (int fs : a1.finalStates) {
        newTransitions[fs] = {Transition('$', {a2.startState + a1.totalStates}, "epsilon")};
    }
    
    return NFA(a1.startState, newTotal, newFinals, newAlphabet, newTransitions);
}

NFA NFA::createClosure(const NFA& a1) {
    int newStart = a1.totalStates;
    int newFinal = a1.totalStates + 1;
    int newTotal = a1.totalStates + 2;
    
    auto newTransitions = a1.transitions;
    auto newAlphabet = a1.alphabet;
    newAlphabet.insert('$');
    
    // Add epsilon transitions from final states back to start and to new final
    for (int fs : a1.finalStates) {
        newTransitions[fs] = {Transition('$', {a1.startState, newFinal}, "epsilon")};
    }
    
    // Add epsilon transition from new start
    newTransitions[newStart] = {Transition('$', {a1.startState, newFinal}, "epsilon")};
    
    return NFA(newStart, newTotal, {newFinal}, newAlphabet, newTransitions);
}