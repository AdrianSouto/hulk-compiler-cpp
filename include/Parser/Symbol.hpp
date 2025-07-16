#pragma once
#include <string>
#include <vector>

namespace Parser {

enum class SymbolType {
    Terminal,
    NonTerminal,
    Epsilon
};

class Symbol {
public:
    std::string value;
    SymbolType type;

    Symbol() : value("ε"), type(SymbolType::Epsilon) {}
    Symbol(const std::string& val, SymbolType t) : value(val), type(t) {}

    bool operator<(const Symbol& other) const {
        return value < other.value;
    }

    bool operator==(const Symbol& other) const {
        return value == other.value;
    }

    bool operator!=(const Symbol& other) const {
        return value != other.value;
    }

    bool isTerminal() const { return type == SymbolType::Terminal; }
    bool isNonTerminal() const { return type == SymbolType::NonTerminal; }
    bool isEpsilon() const { return type == SymbolType::Epsilon; }
};

class Production {
public:
    Symbol left;
    std::vector<Symbol> right;

    Production() {}
    Production(const Symbol& l, const std::vector<Symbol>& r) : left(l), right(r) {}
};

} // namespace Parser