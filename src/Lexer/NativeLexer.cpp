#include "../../include/Lexer/NativeLexer.hpp"
#include <cctype>
#include <stdexcept>

NativeLexer::NativeLexer() : position(0), current_line(1), current_column(1) {
    initializeKeywords();
}

NativeLexer::NativeLexer(const std::string& input) : input(input), position(0), current_line(1), current_column(1) {
    initializeKeywords();
}

void NativeLexer::initializeKeywords() {
    keywords = {

        {"true", TRUE},
        {"false", FALSE},
        

        {"print", PRINT},
        {"let", LET},
        {"in", IN},
        {"function", FUNCTION},
        {"type", TYPE},
        {"inherits", INHERITS},
        {"base", BASE},
        {"new", NEW},
        {"if", IF},
        {"elif", ELIF},
        {"else", ELSE},
        {"while", WHILE},
        {"for", FOR},
        

        {"Number", TYPE_NUMBER},
        {"String", TYPE_STRING},
        {"Boolean", TYPE_BOOLEAN}
    };
}

void NativeLexer::setInput(const std::string& input) {
    this->input = input;
    position = 0;
    current_line = 1;
    current_column = 1;
}

char NativeLexer::peek(size_t offset) const {
    size_t pos = position + offset;
    if (pos >= input.length()) {
        return '\0';
    }
    return input[pos];
}

char NativeLexer::advance() {
    if (position >= input.length()) {
        return '\0';
    }
    
    char c = input[position++];
    if (c == '\n') {
        current_line++;
        current_column = 1;
    } else {
        current_column++;
    }
    return c;
}

void NativeLexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else {
            break;
        }
    }
}

void NativeLexer::skipSingleLineComment() {

    advance();
    advance();
    

    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void NativeLexer::skipMultiLineComment() {
    
    advance(); 
    advance(); 
    
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); 
            advance(); 
            break;
        }
        advance();
    }
}

Token NativeLexer::readNumber() {
    size_t start_pos = position;
    int line = current_line;
    int column = current_column;
    
    while (!isAtEnd() && isDigit(peek())) {
        advance();
    }
    
    std::string number_str = input.substr(start_pos, position - start_pos);
    Token token(NUMBER, number_str, line, column);
    token.number_value = std::stoi(number_str);
    return token;
}

Token NativeLexer::readString() {
    int line = current_line;
    int column = current_column;
    
    advance(); 
    
    std::string value;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            advance(); 
            if (!isAtEnd()) {
                char escaped = advance();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case '"': value += '"'; break;
                    case '\\': value += '\\'; break;
                    default: 
                        value += '\\';
                        value += escaped;
                        break;
                }
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string literal at line " + std::to_string(line));
    }
    
    advance(); 
    
    return Token(STRING, value, line, column);
}

Token NativeLexer::readIdentifier() {
    size_t start_pos = position;
    int line = current_line;
    int column = current_column;
    
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_')) {
        advance();
    }
    
    std::string identifier = input.substr(start_pos, position - start_pos);
    
    
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return Token(it->second, identifier, line, column);
    }
    
    return Token(IDENTIFIER, identifier, line, column);
}

Token NativeLexer::readOperator() {
    int line = current_line;
    int column = current_column;
    char c = advance();
    
    switch (c) {
        case '+': return Token(PLUS, "+", line, column);
        case '-': return Token(MINUS, "-", line, column);
        case '*': return Token(MULTIPLY, "*", line, column);
        case '/': return Token(DIVIDE, "/", line, column);
        case '%': return Token(MODULO, "%", line, column);
        case '^': return Token(POWER, "^", line, column);
        case '&': return Token(AND, "&", line, column);
        case '|': return Token(OR, "|", line, column);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(NOTEQ, "!=", line, column);
            }
            return Token(NOT, "!", line, column);
        case '=':
            if (peek() == '=') {
                advance();
                return Token(EQEQ, "==", line, column);
            } else if (peek() == '>') {
                advance();
                return Token(ARROW, "=>", line, column);
            }
            return Token(EQUALS, "=", line, column);
        case '<':
            if (peek() == '=') {
                advance();
                return Token(LESSEQ, "<=", line, column);
            }
            return Token(LESS, "<", line, column);
        case '>':
            if (peek() == '=') {
                advance();
                return Token(GREATEREQ, ">=", line, column);
            }
            return Token(GREATER, ">", line, column);
        case ':':
            if (peek() == '=') {
                advance();
                return Token(ASSIGN, ":=", line, column);
            }
            return Token(COLON, ":", line, column);
        case '@':
            if (peek() == '@') {
                advance();
                return Token(CONCAT_SPACE, "@@", line, column);
            }
            return Token(CONCAT, "@", line, column);
        case ';': return Token(SEMICOLON, ";", line, column);
        case ',': return Token(COMMA, ",", line, column);
        case '.': return Token(DOT, ".", line, column);
        case '(': return Token(LPAREN, "(", line, column);
        case ')': return Token(RPAREN, ")", line, column);
        case '{': return Token(LBRACE, "{", line, column);
        case '}': return Token(RBRACE, "}", line, column);
        case '\n': return Token(EOL, "\\n", line, column);
        default:
            return Token(UNKNOWN, std::string(1, c), line, column);
    }
}

bool NativeLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool NativeLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool NativeLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

Token NativeLexer::nextToken() {
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) {
            break;
        }
        
        
        if (peek() == '/' && peek(1) == '/') {
            skipSingleLineComment();
            continue;
        }
        
        if (peek() == '/' && peek(1) == '*') {
            skipMultiLineComment();
            continue;
        }
        
        
        char c = peek();
        
        if (isDigit(c)) {
            return readNumber();
        }
        
        if (c == '"') {
            return readString();
        }
        
        if (isAlpha(c)) {
            return readIdentifier();
        }
        
        
        return readOperator();
    }
    
    return Token(EOF_TOKEN, "", current_line, current_column);
}

std::vector<Token> NativeLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type == EOF_TOKEN) {
            break;
        }
        tokens.push_back(token);
    }
    
    tokens.push_back(Token(EOF_TOKEN, "", current_line, current_column));
    return tokens;
}

bool NativeLexer::isAtEnd() const {
    return position >= input.length();
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        case IDENTIFIER: return "IDENTIFIER";
        case TRUE: return "TRUE";
        case FALSE: return "FALSE";
        case PRINT: return "PRINT";
        case LET: return "LET";
        case IN: return "IN";
        case FUNCTION: return "FUNCTION";
        case TYPE: return "TYPE";
        case INHERITS: return "INHERITS";
        case BASE: return "BASE";
        case NEW: return "NEW";
        case IF: return "IF";
        case ELIF: return "ELIF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case FOR: return "FOR";
        case TYPE_NUMBER: return "TYPE_NUMBER";
        case TYPE_STRING: return "TYPE_STRING";
        case TYPE_BOOLEAN: return "TYPE_BOOLEAN";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULTIPLY: return "MULTIPLY";
        case DIVIDE: return "DIVIDE";
        case MODULO: return "MODULO";
        case POWER: return "POWER";
        case CONCAT: return "CONCAT";
        case CONCAT_SPACE: return "CONCAT_SPACE";
        case EQUALS: return "EQUALS";
        case EQEQ: return "EQEQ";
        case NOTEQ: return "NOTEQ";
        case LESS: return "LESS";
        case GREATER: return "GREATER";
        case LESSEQ: return "LESSEQ";
        case GREATEREQ: return "GREATEREQ";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case ASSIGN: return "ASSIGN";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case DOT: return "DOT";
        case ARROW: return "ARROW";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case EOL: return "EOL";
        case EOF_TOKEN: return "EOF_TOKEN";
        case UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}