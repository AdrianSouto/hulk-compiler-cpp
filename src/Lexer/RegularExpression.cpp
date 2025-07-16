#include "Lexer/RegularExpression.hpp"

// EmptyRegEx implementation
bool EmptyRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA EmptyRegEx::toNFA() const {
    return NFA::createEmpty();
}

// EpsilonRegEx implementation
bool EpsilonRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA EpsilonRegEx::toNFA() const {
    return NFA::createEpsilon();
}

// AnyRegEx implementation
bool AnyRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA AnyRegEx::toNFA() const {
    return NFA::createAny();
}

// SymbolRegEx implementation
bool SymbolRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA SymbolRegEx::toNFA() const {
    return NFA::createSymbol(symbol);
}

// RangeRegEx implementation
RangeRegEx::RangeRegEx(char from, char to) 
    : RegularExpression({}), fromChar(from), toChar(to) {
    for (char c = from; c <= to; ++c) {
        alphabet.insert(c);
    }
}

bool RangeRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA RangeRegEx::toNFA() const {
    return NFA::createRange(fromChar, toChar);
}

// UnionRegEx implementation
UnionRegEx::UnionRegEx(RegExPtr l, RegExPtr r) 
    : RegularExpression(l->getAlphabet()), left(l), right(r) {
    const auto& rightAlpha = right->getAlphabet();
    alphabet.insert(rightAlpha.begin(), rightAlpha.end());
}

bool UnionRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA UnionRegEx::toNFA() const {
    return NFA::createUnion(left->toNFA(), right->toNFA());
}

// ConcatenationRegEx implementation
ConcatenationRegEx::ConcatenationRegEx(RegExPtr l, RegExPtr r) 
    : RegularExpression(l->getAlphabet()), left(l), right(r) {
    const auto& rightAlpha = right->getAlphabet();
    alphabet.insert(rightAlpha.begin(), rightAlpha.end());
}

bool ConcatenationRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA ConcatenationRegEx::toNFA() const {
    return NFA::createConcatenation(left->toNFA(), right->toNFA());
}

// ClosureRegEx implementation
bool ClosureRegEx::isValid(const std::string& input) const {
    return toNFA().evaluate(input);
}

NFA ClosureRegEx::toNFA() const {
    return NFA::createClosure(expr->toNFA());
}