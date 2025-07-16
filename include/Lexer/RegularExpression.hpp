#pragma once
#include <memory>
#include <set>
#include <string>
#include "NFA.hpp"

// Base class for regular expressions
class RegularExpression {
protected:
    std::set<char> alphabet;
    
public:
    RegularExpression(const std::set<char>& alpha) : alphabet(alpha) {}
    virtual ~RegularExpression() = default;
    
    virtual bool isValid(const std::string& input) const = 0;
    virtual NFA toNFA() const = 0;
    
    const std::set<char>& getAlphabet() const { return alphabet; }
};

using RegExPtr = std::shared_ptr<RegularExpression>;

// Empty regular expression
class EmptyRegEx : public RegularExpression {
public:
    EmptyRegEx() : RegularExpression({}) {}
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};

// Epsilon regular expression
class EpsilonRegEx : public RegularExpression {
public:
    EpsilonRegEx() : RegularExpression({'$'}) {}
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};

// Any character regular expression
class AnyRegEx : public RegularExpression {
public:
    AnyRegEx() : RegularExpression({'~'}) {}
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};

// Single symbol regular expression
class SymbolRegEx : public RegularExpression {
private:
    char symbol;
    
public:
    SymbolRegEx(char sym) : RegularExpression({sym}), symbol(sym) {}
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};

// Character range regular expression
class RangeRegEx : public RegularExpression {
private:
    char fromChar;
    char toChar;
    
public:
    RangeRegEx(char from, char to);
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};

// Union of two regular expressions
class UnionRegEx : public RegularExpression {
private:
    RegExPtr left;
    RegExPtr right;
    
public:
    UnionRegEx(RegExPtr l, RegExPtr r);
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};

// Concatenation of two regular expressions
class ConcatenationRegEx : public RegularExpression {
private:
    RegExPtr left;
    RegExPtr right;
    
public:
    ConcatenationRegEx(RegExPtr l, RegExPtr r);
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};

// Kleene closure of a regular expression
class ClosureRegEx : public RegularExpression {
private:
    RegExPtr expr;
    
public:
    ClosureRegEx(RegExPtr e) : RegularExpression(e->getAlphabet()), expr(e) {}
    bool isValid(const std::string& input) const override;
    NFA toNFA() const override;
};