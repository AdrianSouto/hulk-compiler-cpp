#include "Lexer/Lexer.hpp"
#include "Lexer/PatternParser.hpp"
#include <cctype>
#include <iostream>
#include <map>

Lexer::Lexer() : currentRow(1), currentCol(1) {
    initializeRecognitionPatterns();
}

void Lexer::initializeRecognitionPatterns() {

    std::vector<std::tuple<TokenKind, std::string>> patterns = {

        {TOKEN_TEXT, "\\\"([^\\\"\n\\\\]|\\\\.)*\\\""},
        

        {TOKEN_NUMERIC, "[0-9]+\\.?[0-9]*"},
        

        {TOKEN_EQUAL, "=="},
        {TOKEN_NOTEQUAL, "!="},
        {TOKEN_LESSEQUAL, "<="},
        {TOKEN_GREATEREQUAL, ">="},
        {TOKEN_LOGICAL_AND, "&&"},
        {TOKEN_JOIN_SPACE, "@@"},
        {TOKEN_IMPLIES, "=>"},
        {TOKEN_EXPONENT, "\\*\\*"},
        {TOKEN_BIND, ":="},
        

        {TOKEN_ADD, "\\+"},
        {TOKEN_SUBTRACT, "\\-"},
        {TOKEN_TIMES, "\\*"},
        {TOKEN_QUOTIENT, "/"},
        {TOKEN_REMAINDER, "%"},
        {TOKEN_JOIN, "@"},
        {TOKEN_MATCH, "="},
        {TOKEN_BELOW, "<"},
        {TOKEN_ABOVE, ">"},
        {TOKEN_NEGATE, "!"},
        {TOKEN_LOGICAL_OR, "\\|"},
        

        {TOKEN_OPEN_PAREN, "\\("},
        {TOKEN_CLOSE_PAREN, "\\)"},
        {TOKEN_OPEN_BRACE, "\\{"},
        {TOKEN_CLOSE_BRACE, "\\}"},
        {TOKEN_TERMINATOR, ";"},
        {TOKEN_SEPARATOR, ","},
        {TOKEN_MARKER, ":"},
        {TOKEN_ACCESSOR, "\\."},
        

        {TOKEN_NAME, "[a-zA-Z][a-zA-Z0-9_]*"},
    };
    
    std::vector<NonDeterministicAutomaton> automatons;
    

    for (const auto& [tokenKind, patternString] : patterns) {
        PatternParser parser(patternString);
        auto patternExpr = parser.parse();
        NonDeterministicAutomaton automaton = patternExpr->toAutomaton();
        

        for (int acceptingState : automaton.getAcceptingStates()) {
            automaton.setTokenKind(acceptingState, tokenKind);
        }
        
        automatons.push_back(automaton);
    }
    

    if (automatons.empty()) return;
    
    NonDeterministicAutomaton combined = automatons[0];
    for (size_t i = 1; i < automatons.size(); i++) {
        combined = NonDeterministicAutomaton::createUnion(combined, automatons[i]);
    }
    

    recognizer = combined.convertToDeterministic();
}

std::vector<Token> Lexer::analyze(const std::string& input) {
    std::vector<Token> tokens;
    size_t position = 0;
    
    while (position < input.size()) {

        if (std::isspace(input[position])) {
            if (input[position] == '\n') {
                currentRow++;
                currentCol = 1;
            } else {
                currentCol++;
            }
            position++;
            continue;
        }
        

        if (position + 1 < input.size() && 
            input[position] == '/' && input[position + 1] == '/') {

            while (position < input.size() && input[position] != '\n') {
                position++;
            }
            continue;
        }
        

        Token token = scanToken(input, position);
        
        if (token.kind == TOKEN_INVALID) {
            std::cerr << "Error: Unrecognized character '" << input[position] 
                     << "' at row " << currentRow << ", column " << currentCol << std::endl;
            position++;
            currentCol++;
        } else {
            tokens.push_back(token);
            position += token.text.length();
            currentCol += token.text.length();
        }
    }
    

    tokens.push_back(Token("", TOKEN_ENDFILE, currentRow, currentCol));
    
    return tokens;
}

Token Lexer::scanToken(const std::string& input, size_t& startPos) {
    size_t currentPos = startPos;
    int currentState = recognizer.getInitialState();
    int lastAcceptingState = -1;
    size_t lastAcceptingPos = startPos;
    

    if (input[currentPos] == '"') {
        currentPos++;
        
        while (currentPos < input.size()) {
            if (input[currentPos] == '"') {
                currentPos++;
                std::string text = input.substr(startPos, currentPos - startPos);
                return Token(text, TOKEN_TEXT, currentRow, currentCol);
            } else if (input[currentPos] == '\\' && currentPos + 1 < input.size()) {

                currentPos += 2;
            } else if (input[currentPos] == '\n') {

                break;
            } else {
                currentPos++;
            }
        }
        

        return Token("", TOKEN_INVALID, currentRow, currentCol);
    }
    

    while (currentPos < input.size()) {
        char ch = input[currentPos];
        

        if (std::isspace(ch) || 
            (ch == '/' && currentPos + 1 < input.size() && input[currentPos + 1] == '/')) {
            break;
        }
        
        int nextState = recognizer.getNextState(ch, currentState);
        
        if (nextState < 0) {

            break;
        }
        
        currentState = nextState;
        currentPos++;
        

        if (recognizer.isAcceptingState(currentState)) {
            lastAcceptingState = currentState;
            lastAcceptingPos = currentPos;
        }
    }
    

    if (lastAcceptingState >= 0) {
        std::string text = input.substr(startPos, lastAcceptingPos - startPos);
        TokenKind kind = recognizer.getTokenKind(lastAcceptingState);
        

        if (kind == TOKEN_NUMERIC) {

            if (lastAcceptingPos < input.size() && 
                (std::isalpha(input[lastAcceptingPos]) || input[lastAcceptingPos] == '_')) {
                return Token("", TOKEN_INVALID, currentRow, currentCol);
            }
        }
        

        if (kind == TOKEN_NAME) {

            static const std::map<std::string, TokenKind> keywords = {
                {"declare", TOKEN_DECLARE},
                {"within", TOKEN_WITHIN},
                {"procedure", TOKEN_PROCEDURE},
                {"class", TOKEN_CLASS},
                {"extends", TOKEN_EXTENDS},
                {"create", TOKEN_CREATE},
                {"super", TOKEN_SUPER},
                {"when", TOKEN_WHEN},
                {"elseif", TOKEN_ELSEIF},
                {"otherwise", TOKEN_OTHERWISE},
                {"loop", TOKEN_LOOP},
                {"iterate", TOKEN_ITERATE},
                {"instanceof", TOKEN_INSTANCEOF},
                {"cast", TOKEN_CAST},
                {"output", TOKEN_OUTPUT},
                {"affirmative", TOKEN_AFFIRMATIVE},
                {"negative", TOKEN_NEGATIVE},
                {"Numeric", TOKEN_KIND_NUMERIC},
                {"Text", TOKEN_KIND_TEXT},
                {"Logical", TOKEN_KIND_LOGICAL},
            };
            
            auto it = keywords.find(text);
            if (it != keywords.end()) {
                kind = it->second;
            }
        }
        
        return Token(text, kind, currentRow, currentCol);
    }
    

    return Token("", TOKEN_INVALID, currentRow, currentCol);
}

void Lexer::reset() {
    currentRow = 1;
    currentCol = 1;
}