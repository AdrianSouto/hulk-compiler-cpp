#pragma once
#include <vector>
#include <string>

struct Transition {
    char character;
    std::vector<int> toStates;
    std::string type; // "normal", "epsilon", "any"
    
    Transition(char c, const std::vector<int>& to, const std::string& t)
        : character(c), toStates(to), type(t) {}
};