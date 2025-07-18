#pragma once
#include <vector>
#include <string>

struct StateTransition {
    char inputChar;
    std::vector<int> destinationStates;
    std::string transitionType; // "standard", "epsilon", "wildcard"
    
    StateTransition(char c, const std::vector<int>& dest, const std::string& type)
        : inputChar(c), destinationStates(dest), transitionType(type) {}
};