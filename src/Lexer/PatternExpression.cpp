#include "Lexer/PatternExpression.hpp"
#include <algorithm>
#include <functional>


bool EmptyPattern::matches(const std::string& input) const {
    return input.empty();
}

NonDeterministicAutomaton EmptyPattern::toAutomaton() const {
    return NonDeterministicAutomaton::createEmpty();
}


bool EpsilonPattern::matches(const std::string& input) const {
    return input.empty();
}

NonDeterministicAutomaton EpsilonPattern::toAutomaton() const {
    return NonDeterministicAutomaton::createEpsilon();
}


bool WildcardPattern::matches(const std::string& input) const {
    return input.length() == 1;
}

NonDeterministicAutomaton WildcardPattern::toAutomaton() const {
    return NonDeterministicAutomaton::createWildcard();
}


bool CharacterPattern::matches(const std::string& input) const {
    return input.length() == 1 && input[0] == character;
}

NonDeterministicAutomaton CharacterPattern::toAutomaton() const {
    return NonDeterministicAutomaton::createCharacter(character);
}


RangePattern::RangePattern(char start, char end) : PatternExpression({}), startChar(start), endChar(end) {
    for (char ch = start; ch <= end; ch++) {
        inputAlphabet.insert(ch);
    }
}

bool RangePattern::matches(const std::string& input) const {
    if (input.length() != 1) return false;
    char ch = input[0];
    return ch >= startChar && ch <= endChar;
}

NonDeterministicAutomaton RangePattern::toAutomaton() const {
    return NonDeterministicAutomaton::createCharacterRange(startChar, endChar);
}


UnionPattern::UnionPattern(PatternPtr left, PatternPtr right) : PatternExpression({}), leftPattern(left), rightPattern(right) {
    inputAlphabet = left->getInputAlphabet();
    const auto& rightAlphabet = right->getInputAlphabet();
    inputAlphabet.insert(rightAlphabet.begin(), rightAlphabet.end());
}

bool UnionPattern::matches(const std::string& input) const {
    return leftPattern->matches(input) || rightPattern->matches(input);
}

NonDeterministicAutomaton UnionPattern::toAutomaton() const {
    return NonDeterministicAutomaton::createUnion(leftPattern->toAutomaton(), rightPattern->toAutomaton());
}


ConcatenationPattern::ConcatenationPattern(PatternPtr left, PatternPtr right) : PatternExpression({}), leftPattern(left), rightPattern(right) {
    inputAlphabet = left->getInputAlphabet();
    const auto& rightAlphabet = right->getInputAlphabet();
    inputAlphabet.insert(rightAlphabet.begin(), rightAlphabet.end());
}

bool ConcatenationPattern::matches(const std::string& input) const {

    for (size_t i = 0; i <= input.length(); i++) {
        std::string leftPart = input.substr(0, i);
        std::string rightPart = input.substr(i);
        
        if (leftPattern->matches(leftPart) && rightPattern->matches(rightPart)) {
            return true;
        }
    }
    return false;
}

NonDeterministicAutomaton ConcatenationPattern::toAutomaton() const {
    return NonDeterministicAutomaton::createConcatenation(leftPattern->toAutomaton(), rightPattern->toAutomaton());
}


bool KleeneStarPattern::matches(const std::string& input) const {
    if (input.empty()) return true;
    

    std::function<bool(const std::string&, size_t)> matchRecursive = 
        [&](const std::string& str, size_t pos) -> bool {
            if (pos == str.length()) return true;
            
            for (size_t len = 1; len <= str.length() - pos; len++) {
                std::string substring = str.substr(pos, len);
                if (pattern->matches(substring) && matchRecursive(str, pos + len)) {
                    return true;
                }
            }
            return false;
        };
    
    return matchRecursive(input, 0);
}

NonDeterministicAutomaton KleeneStarPattern::toAutomaton() const {
    return NonDeterministicAutomaton::createKleeneStar(pattern->toAutomaton());
}