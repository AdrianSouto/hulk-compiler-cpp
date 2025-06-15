#include "../../include/Lexer/FlexCompatibleLexer.hpp"
#include "../../hulk/parser.hpp"
#include <unordered_map>
#include <cctype>
#include <cstring>


char* yytext = nullptr;
int yyleng = 0;
FILE* yyin = nullptr;


extern int line_count;


std::string FlexCompatibleLexer::input_buffer;
size_t FlexCompatibleLexer::current_pos = 0;
size_t FlexCompatibleLexer::buffer_size = 0;
std::string FlexCompatibleLexer::current_token;


static std::unordered_map<std::string, int> keywords = {
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
    {"is", IS},
    {"as", AS},
    {"Number", TYPE_NUMBER},
    {"String", TYPE_STRING},
    {"Boolean", TYPE_BOOLEAN}
};

void FlexCompatibleLexer::setInput(const std::string& input) {
    input_buffer = input;
    buffer_size = input.length();
    current_pos = 0;
    line_count = 1;
}

void FlexCompatibleLexer::setInputFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    setInput(content);
}

void FlexCompatibleLexer::ensureBufferLoaded() {
    if (input_buffer.empty() && yyin != nullptr) {

        char buffer[4096];
        std::string content;
        while (fgets(buffer, sizeof(buffer), yyin)) {
            content += buffer;
        }
        setInput(content);
    }
}

char FlexCompatibleLexer::peek(size_t offset) {
    ensureBufferLoaded();
    size_t pos = current_pos + offset;
    if (pos >= buffer_size) {
        return '\0';
    }
    return input_buffer[pos];
}

char FlexCompatibleLexer::advance() {
    ensureBufferLoaded();
    if (current_pos >= buffer_size) {
        return '\0';
    }
    
    char c = input_buffer[current_pos++];
    if (c == '\n') {
        line_count++;
    }
    return c;
}

void FlexCompatibleLexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else {
            break;
        }
    }
}

void FlexCompatibleLexer::skipSingleLineComment() {

    advance();
    advance();
    

    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void FlexCompatibleLexer::skipMultiLineComment() {

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

bool FlexCompatibleLexer::isAtEnd() {
    ensureBufferLoaded();
    return current_pos >= buffer_size;
}

bool FlexCompatibleLexer::isAlpha(char c) {
    return std::isalpha(c) || c == '_';
}

bool FlexCompatibleLexer::isDigit(char c) {
    return std::isdigit(c);
}

bool FlexCompatibleLexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

std::string FlexCompatibleLexer::readNumber() {
    std::string number;
    while (!isAtEnd() && isDigit(peek())) {
        number += advance();
    }
    

    if (!isAtEnd() && peek() == '.') {
        number += advance();
        

        while (!isAtEnd() && isDigit(peek())) {
            number += advance();
        }
    }
    
    return number;
}

std::string FlexCompatibleLexer::readString() {
    std::string value;
    advance(); 
    
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
        throw std::runtime_error("Unterminated string literal at line " + std::to_string(line_count));
    }
    
    advance(); 
    return value;
}

std::string FlexCompatibleLexer::readIdentifier() {
    std::string identifier;
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_')) {
        identifier += advance();
    }
    return identifier;
}

int FlexCompatibleLexer::checkKeyword(const std::string& text) {
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return it->second;
    }
    return IDENTIFIER;
}

int FlexCompatibleLexer::getNextToken() {
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
        
        
        if (peek() == '\n') {
            advance();
            continue; 
        }
        
        char c = peek();
        
        
        if (isDigit(c)) {
            current_token = readNumber();
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = current_token.length();
            

            if (current_token.find('.') != std::string::npos) {
                yylval.decimal = std::stod(current_token);
            } else {
                yylval.number = std::stoi(current_token);
            }
            return NUMBER;
        }
        
        
        if (c == '"') {
            std::string str_content = readString();
            current_token = "\"" + str_content + "\""; 
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = current_token.length();
            yylval.string = strdup(str_content.c_str());
            return STRING;
        }
        
        
        if (isAlpha(c)) {
            current_token = readIdentifier();
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = current_token.length();
            
            int token_type = checkKeyword(current_token);
            if (token_type == IDENTIFIER) {
                yylval.string = strdup(current_token.c_str());
            }
            return token_type;
        }
        
        
        if (c == '=' && peek(1) == '>') {
            advance(); advance();
            current_token = "=>";
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = 2;
            return ARROW;
        }
        
        if (c == ':' && peek(1) == '=') {
            advance(); advance();
            current_token = ":=";
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = 2;
            return ASSIGN;
        }
        
        if (c == '=' && peek(1) == '=') {
            advance(); advance();
            current_token = "==";
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = 2;
            return EQEQ;
        }
        
        if (c == '!' && peek(1) == '=') {
            advance(); advance();
            current_token = "!=";
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = 2;
            return NOTEQ;
        }
        
        if (c == '<' && peek(1) == '=') {
            advance(); advance();
            current_token = "<=";
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = 2;
            return LESSEQ;
        }
        
        if (c == '>' && peek(1) == '=') {
            advance(); advance();
            current_token = ">=";
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = 2;
            return GREATEREQ;
        }
        
        if (c == '@' && peek(1) == '@') {
            advance(); advance();
            current_token = "@@";
            yytext = const_cast<char*>(current_token.c_str());
            yyleng = 2;
            return CONCAT_SPACE;
        }
        
        
        advance();
        current_token = std::string(1, c);
        yytext = const_cast<char*>(current_token.c_str());
        yyleng = 1;
        
        switch (c) {
            case ';': return SEMICOLON;
            case '=': return EQUALS;
            case ',': return COMMA;
            case ':': return COLON;
            case '(': return LPAREN;
            case ')': return RPAREN;
            case '+': return PLUS;
            case '-': return MINUS;
            case '*': return MULTIPLY;
            case '/': return DIVIDE;
            case '%': return MODULO;
            case '^': return POWER;
            case '@': return CONCAT;
            case '{': return LBRACE;
            case '}': return RBRACE;
            case '<': return LESS;
            case '>': return GREATER;
            case '&': return AND;
            case '|': return OR;
            case '!': return NOT;
            case '.': return DOT;
            default:
                printf("Carácter inesperado: '%c' en línea %d\n", c, line_count);
                continue; 
        }
    }
    
    return 0; 
}

void FlexCompatibleLexer::reset() {
    current_pos = 0;
    line_count = 1;
}


int yylex() {
    return FlexCompatibleLexer::getNextToken();
}