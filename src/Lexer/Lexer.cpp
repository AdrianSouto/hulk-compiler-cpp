#include "Lexer/Lexer.hpp"
#include "Lexer/RegexParser.hpp"
#include <cctype>
#include <iostream>
#include <map>

Lexer::Lexer() : currentLine(1), currentColumn(1) {
    initializePatterns();
}

void Lexer::initializePatterns() {
    // Define token patterns in order of priority
    std::vector<std::tuple<TokenType, std::string>> patterns = {
        // String literals (highest priority)
        {TOKEN_STRING, "\\\"([^\\\"\n\\\\]|\\\\.)*\\\""},
        
        // Numbers (decimal and integer)
        {TOKEN_NUMBER, "[0-9]+\\.?[0-9]*"},
        
        // Multi-character operators (before single-char to avoid conflicts)
        {TOKEN_EQEQ, "=="},
        {TOKEN_NOTEQ, "!="},
        {TOKEN_LESSEQ, "<="},
        {TOKEN_GREATEREQ, ">="},
        {TOKEN_AND, "&&"},
        {TOKEN_CONCAT_SPACE, "@@"},
        {TOKEN_ARROW, "=>"},
        {TOKEN_POWER, "\\*\\*"},
        {TOKEN_ASSIGN, ":="},
        
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
        
        // Identifiers (lowest priority among named tokens)
        {TOKEN_IDENTIFIER, "[a-zA-Z][a-zA-Z0-9_]*"},
    };
    
    std::vector<NFA> nfas;
    
    // Create NFA for each pattern
    for (const auto& [tokenType, pattern] : patterns) {
        RegexParser parser(pattern);
        auto regex = parser.parse();
        NFA nfa = regex->toNFA();
        
        // Set token type for final states
        for (int finalState : nfa.getFinalStates()) {
            nfa.setTokenType(finalState, tokenType);
        }
        
        nfas.push_back(nfa);
    }
    
    // Combine all NFAs into one
    if (nfas.empty()) return;
    
    NFA combined = nfas[0];
    for (size_t i = 1; i < nfas.size(); i++) {
        combined = NFA::createUnion(combined, nfas[i]);
    }
    
    // Convert to DFA for efficient matching
    automaton = combined.convertToDFA();
}

std::vector<Token> Lexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t position = 0;
    
    while (position < input.size()) {
        // Skip whitespace
        if (std::isspace(input[position])) {
            if (input[position] == '\n') {
                currentLine++;
                currentColumn = 1;
            } else {
                currentColumn++;
            }
            position++;
            continue;
        }
        
        // Skip single-line comments
        if (position + 1 < input.size() && 
            input[position] == '/' && input[position + 1] == '/') {
            // Skip until end of line
            while (position < input.size() && input[position] != '\n') {
                position++;
            }
            continue;
        }
        
        // Scan next token
        Token token = scanToken(input, position);
        
        if (token.type == TOKEN_ERROR) {
            std::cerr << "Error: Unrecognized character '" << input[position] 
                     << "' at line " << currentLine << ", column " << currentColumn << std::endl;
            position++;
            currentColumn++;
        } else {
            tokens.push_back(token);
            position += token.lexeme.length();
            currentColumn += token.lexeme.length();
        }
    }
    
    // Add EOF token
    tokens.push_back(Token("", TOKEN_EOF, currentLine, currentColumn));
    
    return tokens;
}

Token Lexer::scanToken(const std::string& input, size_t& startPos) {
    size_t currentPos = startPos;
    int currentState = automaton.getStartState();
    int lastFinalState = -1;
    size_t lastFinalPos = startPos;
    
    // Handle string literals specially
    if (input[currentPos] == '"') {
        currentPos++;
        size_t stringStart = currentPos;
        
        while (currentPos < input.size()) {
            if (input[currentPos] == '"') {
                currentPos++;
                std::string lexeme = input.substr(startPos, currentPos - startPos);
                return Token(lexeme, TOKEN_STRING, currentLine, currentColumn);
            } else if (input[currentPos] == '\\' && currentPos + 1 < input.size()) {
                // Skip escape sequence
                currentPos += 2;
            } else if (input[currentPos] == '\n') {
                // Unterminated string
                break;
            } else {
                currentPos++;
            }
        }
        
        // Error: unterminated string
        return Token("", TOKEN_ERROR, currentLine, currentColumn);
    }
    
    // Regular DFA matching
    while (currentPos < input.size()) {
        char ch = input[currentPos];
        
        // Check for whitespace or comment start
        if (std::isspace(ch) || 
            (ch == '/' && currentPos + 1 < input.size() && input[currentPos + 1] == '/')) {
            break;
        }
        
        int nextState = automaton.getNextState(ch, currentState);
        
        if (nextState < 0) {
            // No valid transition
            break;
        }
        
        currentState = nextState;
        currentPos++;
        
        // Check if current state is final
        if (automaton.isFinalState(currentState)) {
            lastFinalState = currentState;
            lastFinalPos = currentPos;
        }
    }
    
    // Check if we found a valid token
    if (lastFinalState >= 0) {
        std::string lexeme = input.substr(startPos, lastFinalPos - startPos);
        TokenType type = automaton.getTokenType(lastFinalState);
        
        // Special handling for numbers
        if (type == TOKEN_NUMBER) {
            // Check if next character invalidates the number
            if (lastFinalPos < input.size() && 
                (std::isalpha(input[lastFinalPos]) || input[lastFinalPos] == '_')) {
                return Token("", TOKEN_ERROR, currentLine, currentColumn);
            }
        }
        
        // Check if identifier is actually a keyword
        if (type == TOKEN_IDENTIFIER) {
            // Map of keywords
            static const std::map<std::string, TokenType> keywords = {
                {"let", TOKEN_LET},
                {"in", TOKEN_IN},
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
                {"is", TOKEN_IS},
                {"as", TOKEN_AS},
                {"print", TOKEN_PRINT},
                {"true", TOKEN_TRUE},
                {"false", TOKEN_FALSE},
                {"Number", TOKEN_TYPE_NUMBER},
                {"String", TOKEN_TYPE_STRING},
                {"Boolean", TOKEN_TYPE_BOOLEAN}
            };
            
            auto it = keywords.find(lexeme);
            if (it != keywords.end()) {
                type = it->second;
            }
        }
        
        return Token(lexeme, type, currentLine, currentColumn);
    }
    
    // No valid token found
    return Token("", TOKEN_ERROR, currentLine, currentColumn);
}

void Lexer::reset() {
    currentLine = 1;
    currentColumn = 1;
}