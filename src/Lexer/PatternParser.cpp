#include "Lexer/PatternParser.hpp"
#include <stdexcept>
#include <memory>

PatternPtr PatternParser::parse() {
    position = 0;
    return parseExpression();
}

PatternPtr PatternParser::parseExpression() {
    PatternPtr left = parseTerm();
    
    while (!isAtEnd() && peek() == '|') {
        advance();
        PatternPtr right = parseTerm();
        left = std::make_shared<UnionPattern>(left, right);
    }
    
    return left;
}

PatternPtr PatternParser::parseTerm() {
    PatternPtr left = parseFactor();
    
    while (!isAtEnd() && peek() != '|' && peek() != ')') {
        PatternPtr right = parseFactor();
        left = std::make_shared<ConcatenationPattern>(left, right);
    }
    
    return left;
}

PatternPtr PatternParser::parseFactor() {
    PatternPtr atom = parseAtom();
    
    if (!isAtEnd()) {
        char ch = peek();
        if (ch == '*') {
            advance();
            return std::make_shared<KleeneStarPattern>(atom);
        } else if (ch == '+') {
            advance();

            return std::make_shared<ConcatenationPattern>(
                atom, 
                std::make_shared<KleeneStarPattern>(atom)
            );
        } else if (ch == '?') {
            advance();

            return std::make_shared<UnionPattern>(
                atom,
                std::make_shared<EpsilonPattern>()
            );
        }
    }
    
    return atom;
}

PatternPtr PatternParser::parseAtom() {
    if (isAtEnd()) {
        throw std::runtime_error("Unexpected end of pattern");
    }
    
    char ch = peek();
    
    if (ch == '(') {
        advance();
        PatternPtr expr = parseExpression();
        if (!match(')')) {
            throw std::runtime_error("Expected ')' after grouped expression");
        }
        return expr;
    } else if (ch == '[') {
        return parseCharacterClass();
    } else if (ch == '\\') {
        return parseEscape();
    } else if (ch == '.') {
        advance();
        return std::make_shared<WildcardPattern>();
    } else {
        advance();
        return std::make_shared<CharacterPattern>(ch);
    }
}

PatternPtr PatternParser::parseCharacterClass() {
    if (!match('[')) {
        throw std::runtime_error("Expected '[' at start of character class");
    }
    
    bool negated = false;
    if (peek() == '^') {
        negated = true;
        advance();
    }
    
    std::vector<PatternPtr> patterns;
    
    while (!isAtEnd() && peek() != ']') {
        char start = advance();
        
        if (!isAtEnd() && peek() == '-' && position + 1 < pattern.length() && pattern[position + 1] != ']') {
            advance();
            char end = advance();
            patterns.push_back(std::make_shared<RangePattern>(start, end));
        } else {
            patterns.push_back(std::make_shared<CharacterPattern>(start));
        }
    }
    
    if (!match(']')) {
        throw std::runtime_error("Expected ']' at end of character class");
    }
    
    if (patterns.empty()) {
        throw std::runtime_error("Empty character class");
    }
    

    PatternPtr result = patterns[0];
    for (size_t i = 1; i < patterns.size(); i++) {
        result = std::make_shared<UnionPattern>(result, patterns[i]);
    }
    

    if (negated) {


    }
    
    return result;
}

PatternPtr PatternParser::parseEscape() {
    if (!match('\\')) {
        throw std::runtime_error("Expected '\\' at start of escape sequence");
    }
    
    if (isAtEnd()) {
        throw std::runtime_error("Unexpected end after escape character");
    }
    
    char ch = advance();
    
    switch (ch) {
        case 'n': return std::make_shared<CharacterPattern>('\n');
        case 't': return std::make_shared<CharacterPattern>('\t');
        case 'r': return std::make_shared<CharacterPattern>('\r');
        case '\\': return std::make_shared<CharacterPattern>('\\');
        case '.': return std::make_shared<CharacterPattern>('.');
        case '*': return std::make_shared<CharacterPattern>('*');
        case '+': return std::make_shared<CharacterPattern>('+');
        case '?': return std::make_shared<CharacterPattern>('?');
        case '(': return std::make_shared<CharacterPattern>('(');
        case ')': return std::make_shared<CharacterPattern>(')');
        case '[': return std::make_shared<CharacterPattern>('[');
        case ']': return std::make_shared<CharacterPattern>(']');
        case '|': return std::make_shared<CharacterPattern>('|');
        case '^': return std::make_shared<CharacterPattern>('^');
        case '$': return std::make_shared<CharacterPattern>('$');
        case 'd': return std::make_shared<RangePattern>('0', '9');
        case 'w': {

            auto letters = std::make_shared<UnionPattern>(
                std::make_shared<RangePattern>('a', 'z'),
                std::make_shared<RangePattern>('A', 'Z')
            );
            auto alphanumeric = std::make_shared<UnionPattern>(
                letters,
                std::make_shared<RangePattern>('0', '9')
            );
            return std::make_shared<UnionPattern>(
                alphanumeric,
                std::make_shared<CharacterPattern>('_')
            );
        }
        case 's': {

            auto space = std::make_shared<CharacterPattern>(' ');
            auto tab = std::make_shared<CharacterPattern>('\t');
            auto newline = std::make_shared<CharacterPattern>('\n');
            auto carriage = std::make_shared<CharacterPattern>('\r');
            
            auto spaceTab = std::make_shared<UnionPattern>(space, tab);
            auto newlines = std::make_shared<UnionPattern>(newline, carriage);
            return std::make_shared<UnionPattern>(spaceTab, newlines);
        }
        default:
            return std::make_shared<CharacterPattern>(ch);
    }
}

char PatternParser::peek() {
    if (isAtEnd()) return '\0';
    return pattern[position];
}

char PatternParser::advance() {
    if (isAtEnd()) return '\0';
    return pattern[position++];
}

bool PatternParser::isAtEnd() {
    return position >= pattern.length();
}

bool PatternParser::match(char expected) {
    if (isAtEnd() || peek() != expected) {
        return false;
    }
    advance();
    return true;
}