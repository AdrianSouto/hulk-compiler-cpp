#include "Lexer/Lexer.hpp"
#include "Lexer/PatternParser.hpp"
#include <cctype>
#include <iostream>
#include <map>

Lexer::Lexer() : currentRow(1), currentCol(1) {
    initializeRecognitionPatterns();
}

void Lexer::initializeRecognitionPatterns() {
    std::vector<std::tuple<TokenType, std::string>> patterns = {
        // String literals
        {TOKEN_STRING, "\\\"([^\\\"\n\\\\]|\\\\.)*\\\""},
        
        // Numbers
        {TOKEN_NUMBER, "[0-9]+\\.?[0-9]*"},
        
        // Multi-character operators (order matters - longer first)
        {TOKEN_CONCAT_SPACE, "@@"},
        {TOKEN_POWER, "\\*\\*"},
        {TOKEN_ASSIGN, ":="},
        {TOKEN_EQEQ, "=="},
        {TOKEN_NOTEQ, "!="},
        {TOKEN_LESSEQ, "<="},
        {TOKEN_GREATEREQ, ">="},
        {TOKEN_AND, "&&"},
        {TOKEN_ARROW, "=>"},
        
        // Single character operators
        {TOKEN_PLUS, "\\+"},
        {TOKEN_MINUS, "\\-"},
        {TOKEN_MULTIPLY, "\\*"},
        {TOKEN_DIVIDE, "/"},
        {TOKEN_MODULO, "%"},
        {TOKEN_CONCAT, "@"},
        {TOKEN_EQUALS, "="},
        {TOKEN_LESS, "<"},
        {TOKEN_GREATER, ">"},
        {TOKEN_NOT, "!"},
        {TOKEN_OR, "\\|"},
        
        // Delimiters
        {TOKEN_LPAREN, "\\("},
        {TOKEN_RPAREN, "\\)"},
        {TOKEN_LBRACE, "\\{"},
        {TOKEN_RBRACE, "\\}"},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_COMMA, ","},
        {TOKEN_COLON, ":"},
        {TOKEN_DOT, "\\."},
        
        // Identifiers (must be last to allow keywords to match first)
        {TOKEN_IDENTIFIER, "[a-zA-Z][a-zA-Z0-9_]*"},
    };
    
    std::vector<NonDeterministicAutomaton> automatons;
    
    for (const auto& [tokenType, patternString] : patterns) {
        PatternParser parser(patternString);
        auto patternExpr = parser.parse();
        NonDeterministicAutomaton automaton = patternExpr->toAutomaton();
        
        for (int acceptingState : automaton.getAcceptingStates()) {
            automaton.setTokenType(acceptingState, tokenType);
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
        // Skip whitespace
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
        
        // Skip comments
        if (position + 1 < input.size() && 
            input[position] == '/' && input[position + 1] == '/') {
            while (position < input.size() && input[position] != '\n') {
                position++;
            }
            continue;
        }
        
        Token token = scanToken(input, position);
        
        if (token.type == TOKEN_INVALID) {
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
    
    // Add EOF token
    Token eofToken("", TOKEN_EOF, currentRow, currentCol);
    tokens.push_back(eofToken);
    
    return tokens;
}

Token Lexer::scanToken(const std::string& input, size_t& startPos) {
    size_t currentPos = startPos;
    int currentState = recognizer.getInitialState();
    int lastAcceptingState = -1;
    size_t lastAcceptingPos = startPos;
    
    // Handle string literals specially
    if (input[currentPos] == '"') {
        currentPos++;
        
        while (currentPos < input.size()) {
            if (input[currentPos] == '"') {
                currentPos++;
                std::string text = input.substr(startPos, currentPos - startPos);
                return Token(text, TOKEN_STRING, currentRow, currentCol);
            } else if (input[currentPos] == '\\' && currentPos + 1 < input.size()) {
                // Handle escape sequences
                currentPos += 2;
            } else if (input[currentPos] == '\n') {
                // Unterminated string
                break;
            } else {
                currentPos++;
            }
        }
        
        // Unterminated string
        return Token("", TOKEN_INVALID, currentRow, currentCol);
    }
    
    // Use automaton for other tokens
    while (currentPos < input.size()) {
        char ch = input[currentPos];
        
        // Stop at whitespace or comments
        if (std::isspace(ch) || 
            (ch == '/' && currentPos + 1 < input.size() && input[currentPos + 1] == '/')) {
            break;
        }
        
        int nextState = recognizer.getNextState(ch, currentState);
        
        if (nextState < 0) {
            // No valid transition
            break;
        }
        
        currentState = nextState;
        currentPos++;
        
        // Check if current state is accepting
        if (recognizer.isAcceptingState(currentState)) {
            lastAcceptingState = currentState;
            lastAcceptingPos = currentPos;
        }
    }
    
    // If we found a valid token
    if (lastAcceptingState >= 0) {
        std::string text = input.substr(startPos, lastAcceptingPos - startPos);
        TokenType type = recognizer.getTokenType(lastAcceptingState);
        
        // Check for keywords if this is an identifier
        if (type == TOKEN_IDENTIFIER) {
            static const std::map<std::string, TokenType> keywords = {
                {"function", TOKEN_FUNCTION},
                {"type", TOKEN_TYPE},
                {"inherits", TOKEN_INHERITS},
                {"new", TOKEN_NEW},
                {"base", TOKEN_BASE},
                {"if", TOKEN_IF},
                {"elif", TOKEN_ELIF},
                {"else", TOKEN_ELSE},
                {"while", TOKEN_WHILE},
                {"for", TOKEN_FOR},
                {"in", TOKEN_IN},
                {"is", TOKEN_IS},
                {"as", TOKEN_AS},
                {"let", TOKEN_LET},
                {"print", TOKEN_PRINT},
                {"true", TOKEN_TRUE},
                {"false", TOKEN_FALSE},
                {"Number", TOKEN_TYPE_NUMBER},
                {"String", TOKEN_TYPE_STRING},
                {"Boolean", TOKEN_TYPE_BOOLEAN},
            };
            
            auto it = keywords.find(text);
            if (it != keywords.end()) {
                type = it->second;
            }
        }
        
        // Validate numeric tokens
        if (type == TOKEN_NUMBER) {
            // Make sure number is not followed by letter or underscore
            if (lastAcceptingPos < input.size() && 
                (std::isalpha(input[lastAcceptingPos]) || input[lastAcceptingPos] == '_')) {
                return Token("", TOKEN_INVALID, currentRow, currentCol);
            }
        }
        
        return Token(text, type, currentRow, currentCol);
    }
    
    // No valid token found
    return Token("", TOKEN_INVALID, currentRow, currentCol);
}

void Lexer::reset() {
    currentRow = 1;
    currentCol = 1;
}