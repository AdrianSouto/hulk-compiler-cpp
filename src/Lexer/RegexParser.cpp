#include "Lexer/RegexParser.hpp"
#include <stdexcept>

RegExPtr RegexParser::parse() {
    auto result = parseUnion();
    if (position != input.size()) {
        throw std::runtime_error("Unexpected character at end of regex");
    }
    return result;
}

RegExPtr RegexParser::parseUnion() {
    auto left = parseConcatenation();
    if (consume('|')) {
        auto right = parseUnion();
        return std::make_shared<UnionRegEx>(left, right);
    }
    return left;
}

RegExPtr RegexParser::parseConcatenation() {
    std::vector<RegExPtr> parts;
    
    while (true) {
        char c = peek();
        if (!c || c == '|' || c == ')') break;
        parts.push_back(parseClosure());
    }
    
    if (parts.empty()) {
        return std::make_shared<EmptyRegEx>();
    }
    
    RegExPtr result = parts[0];
    for (size_t i = 1; i < parts.size(); i++) {
        result = std::make_shared<ConcatenationRegEx>(result, parts[i]);
    }
    
    return result;
}

RegExPtr RegexParser::parseClosure() {
    auto atom = parseAtom();
    
    while (true) {
        char c = peek();
        if (c == '*') {
            get();
            atom = std::make_shared<ClosureRegEx>(atom);
        } else if (c == '+') {
            get();
            // a+ is equivalent to aa*
            atom = std::make_shared<ConcatenationRegEx>(
                atom, 
                std::make_shared<ClosureRegEx>(atom)
            );
        } else if (c == '?') {
            get();
            // a? is equivalent to (a|ε)
            auto epsilon = std::make_shared<EpsilonRegEx>();
            atom = std::make_shared<UnionRegEx>(atom, epsilon);
        } else {
            break;
        }
    }
    
    return atom;
}

RegExPtr RegexParser::parseAtom() {
    if (consume('(')) {
        auto result = parseUnion();
        if (!consume(')')) {
            throw std::runtime_error("Unclosed parenthesis in regex");
        }
        return result;
    }
    else if (consume('[')) {
        // Character class
        bool negated = false;
        if (peek() == '^') {
            negated = true;
            get();
        }
        
        std::vector<RegExPtr> ranges;
        
        while (peek() && peek() != ']') {
            char first = get();
            if (peek() == '-' && position + 1 < input.size() && input[position + 1] != ']') {
                get(); // consume '-'
                char last = get();
                if (first > last) {
                    throw std::runtime_error("Invalid range in character class");
                }
                ranges.push_back(std::make_shared<RangeRegEx>(first, last));
            } else {
                ranges.push_back(std::make_shared<SymbolRegEx>(first));
            }
        }
        
        if (!consume(']')) {
            throw std::runtime_error("Unclosed character class");
        }
        
        if (ranges.empty()) {
            return std::make_shared<EmptyRegEx>();
        }
        
        // Create union of all ranges
        RegExPtr result = ranges[0];
        for (size_t i = 1; i < ranges.size(); ++i) {
            result = std::make_shared<UnionRegEx>(result, ranges[i]);
        }
        
        return result;
    }
    else if (consume('"')) {
        // String literal
        std::vector<RegExPtr> chars;
        
        while (peek() && peek() != '"') {
            char c = get();
            if (c == '\\') {
                if (position >= input.size()) {
                    throw std::runtime_error("Dangling backslash in string literal");
                }
                
                char escaped = get();
                switch (escaped) {
                    case 'n': c = '\n'; break;
                    case 't': c = '\t'; break;
                    case 'r': c = '\r'; break;
                    case '"': c = '"'; break;
                    case '\\': c = '\\'; break;
                    default:
                        throw std::runtime_error("Unknown escape sequence in string literal");
                }
            }
            chars.push_back(std::make_shared<SymbolRegEx>(c));
        }
        
        if (!consume('"')) {
            throw std::runtime_error("Unclosed string literal");
        }
        
        if (chars.empty()) {
            return std::make_shared<EpsilonRegEx>();
        }
        
        RegExPtr result = chars[0];
        for (size_t i = 1; i < chars.size(); ++i) {
            result = std::make_shared<ConcatenationRegEx>(result, chars[i]);
        }
        
        return result;
    }
    else if (consume('.')) {
        // Any character
        return std::make_shared<AnyRegEx>();
    }
    else if (peek()) {
        char c = get();
        if (c == '\\') {
            if (position >= input.size()) {
                throw std::runtime_error("Dangling backslash in regex");
            }
            char next = get();
            return std::make_shared<SymbolRegEx>(next);
        }
        return std::make_shared<SymbolRegEx>(c);
    }
    
    throw std::runtime_error("Unexpected end of regex");
}

char RegexParser::peek() const {
    return position < input.size() ? input[position] : '\0';
}

char RegexParser::get() {
    return position < input.size() ? input[position++] : '\0';
}

bool RegexParser::consume(char c) {
    if (peek() == c) {
        position++;
        return true;
    }
    return false;
}