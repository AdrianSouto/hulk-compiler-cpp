#pragma once
#include <memory>
#include <set>
#include <string>
#include "NonDeterministicAutomaton.hpp"

// Base class for pattern expressions
class PatternExpression {
protected:
    std::set<char> inputAlphabet;
    
public:
    PatternExpression(const std::set<char>& alphabet) : inputAlphabet(alphabet) {}
    virtual ~PatternExpression() = default;
    
    virtual bool matches(const std::string& input) const = 0;
    virtual NonDeterministicAutomaton toAutomaton() const = 0;
    
    const std::set<char>& getInputAlphabet() const { return inputAlphabet; }
};

using PatternPtr = std::shared_ptr<PatternExpression>;

// Empty pattern expression
class EmptyPattern : public PatternExpression {
public:
    EmptyPattern() : PatternExpression({}) {}
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};

// Epsilon pattern expression
class EpsilonPattern : public PatternExpression {
public:
    EpsilonPattern() : PatternExpression({'$'}) {}
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};

// Wildcard character pattern expression
class WildcardPattern : public PatternExpression {
public:
    WildcardPattern() : PatternExpression({'~'}) {}
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};

// Single character pattern expression
class CharacterPattern : public PatternExpression {
private:
    char character;
    
public:
    CharacterPattern(char ch) : PatternExpression({ch}), character(ch) {}
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};

// Character range pattern expression
class RangePattern : public PatternExpression {
private:
    char startChar;
    char endChar;
    
public:
    RangePattern(char start, char end);
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};

// Union of two pattern expressions
class UnionPattern : public PatternExpression {
private:
    PatternPtr leftPattern;
    PatternPtr rightPattern;
    
public:
    UnionPattern(PatternPtr left, PatternPtr right);
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};

// Concatenation of two pattern expressions
class ConcatenationPattern : public PatternExpression {
private:
    PatternPtr leftPattern;
    PatternPtr rightPattern;
    
public:
    ConcatenationPattern(PatternPtr left, PatternPtr right);
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};

// Kleene star of a pattern expression
class KleeneStarPattern : public PatternExpression {
private:
    PatternPtr pattern;
    
public:
    KleeneStarPattern(PatternPtr p) : PatternExpression(p->getInputAlphabet()), pattern(p) {}
    bool matches(const std::string& input) const override;
    NonDeterministicAutomaton toAutomaton() const override;
};