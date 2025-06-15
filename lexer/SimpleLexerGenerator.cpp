#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

struct TokenRule {
    std::string pattern;
    std::string action;
    std::string tokenName;
};

class SimpleLexerGenerator {
private:
    std::vector<TokenRule> rules;
    std::string headerCode;
    std::map<std::string, int> tokenValues;
    
    void parseTokensFromParser() {
        std::ifstream file("../parser.y");
        if (!file.is_open()) return;
        
        std::string line;
        int tokenValue = 256;
        
        while (std::getline(file, line)) {
            if (line.find("%token") != std::string::npos) {
                std::istringstream iss(line);
                std::string word;
                bool foundToken = false;
                
                while (iss >> word) {
                    if (foundToken && word.find('<') == std::string::npos) {
                        tokenValues[word] = tokenValue++;
                    } else if (word == "%token" || word.find("%token<") == 0) {
                        foundToken = true;
                    }
                }
            }
        }
        file.close();
    }
    
    void parseLexerFile() {
        std::ifstream file("../lexer.l");
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open ../lexer.l" << std::endl;
            return;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Find sections
        size_t firstSeparator = content.find("%%");
        size_t secondSeparator = content.find("%%", firstSeparator + 2);
        
        if (firstSeparator == std::string::npos || secondSeparator == std::string::npos) {
            std::cerr << "Error: Invalid flex file format" << std::endl;
            return;
        }
        
        // Extract header code
        std::string definitionSection = content.substr(0, firstSeparator);
        std::string rulesSection = content.substr(firstSeparator + 2, 
                                                 secondSeparator - firstSeparator - 2);
        
        // Parse definition section
        std::istringstream defStream(definitionSection);
        std::string line;
        bool inHeader = false;
        
        while (std::getline(defStream, line)) {
            if (line == "%{") {
                inHeader = true;
                continue;
            } else if (line == "%}") {
                inHeader = false;
                continue;
            }
            
            if (inHeader) {
                headerCode += line + "\n";
            }
        }
        
        // Parse rules section
        std::istringstream rulesStream(rulesSection);
        while (std::getline(rulesStream, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line.substr(0, 2) == "/*" || line.substr(0, 2) == "//") continue;
            
            // Find pattern and action
            size_t actionStart = line.find_first_of(" \t{");
            if (actionStart != std::string::npos) {
                std::string pattern = line.substr(0, actionStart);
                std::string action = line.substr(actionStart);
                
                // Extract action from braces
                size_t braceStart = action.find('{');
                size_t braceEnd = action.find_last_of('}');
                if (braceStart != std::string::npos && braceEnd != std::string::npos) {
                    action = action.substr(braceStart + 1, braceEnd - braceStart - 1);
                }
                
                // Clean up action
                action.erase(0, action.find_first_not_of(" \t\n"));
                action.erase(action.find_last_not_of(" \t\n") + 1);
                
                TokenRule rule;
                rule.pattern = pattern;
                rule.action = action;
                
                // Extract token name from return statement
                if (action.find("return") != std::string::npos) {
                    size_t returnPos = action.find("return");
                    size_t tokenStart = action.find_first_not_of(" \t", returnPos + 6);
                    size_t tokenEnd = action.find_first_of(" \t;", tokenStart);
                    if (tokenEnd == std::string::npos) tokenEnd = action.length();
                    
                    rule.tokenName = action.substr(tokenStart, tokenEnd - tokenStart);
                }
                
                rules.push_back(rule);
            }
        }
    }
    
    std::string generateHeader() {
        std::string result = R"(#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <iostream>

)";
        
        result += headerCode + "\n";
        
        result += R"(
extern int line_count;
extern char* yytext;

int yylex();
void setInput(const std::string& input);

#endif // LEXER_HPP
)";
        
        return result;
    }
    
    std::string generateImplementation() {
        std::string result = R"(#include "lexer.hpp"
#include "parser.hpp"
#include <cstring>
#include <cstdlib>
#include <cctype>

static std::string inputText;
static size_t inputPos = 0;
extern int line_count;
char* yytext = nullptr;
extern YYSTYPE yylval;

void setInput(const std::string& input) {
    inputText = input;
    inputPos = 0;
    line_count = 1;
    if (yytext) {
        free(yytext);
        yytext = nullptr;
    }
}

static char peek(size_t offset = 0) {
    if (inputPos + offset >= inputText.length()) {
        return '\0';
    }
    return inputText[inputPos + offset];
}

static void advance(size_t count = 1) {
    for (size_t i = 0; i < count && inputPos < inputText.length(); i++) {
        if (inputText[inputPos] == '\n') {
            line_count++;
        }
        inputPos++;
    }
}

static bool matchString(const std::string& str) {
    if (inputPos + str.length() > inputText.length()) {
        return false;
    }
    return inputText.substr(inputPos, str.length()) == str;
}

int yylex() {
    // Skip whitespace (except newlines which are handled separately)
    while (inputPos < inputText.length() && (peek() == ' ' || peek() == '\t')) {
        advance();
    }
    
    if (inputPos >= inputText.length()) {
        return 0; // EOF
    }
    
    // Free previous yytext
    if (yytext) {
        free(yytext);
        yytext = nullptr;
    }
    
    // Handle comments first
    if (matchString("//")) {
        // Single-line comment
        while (inputPos < inputText.length() && peek() != '\n') {
            advance();
        }
        return yylex(); // Continue to next token
    }
    
    if (matchString("/*")) {
        // Multi-line comment
        advance(2);
        while (inputPos < inputText.length() - 1) {
            if (peek() == '*' && peek(1) == '/') {
                advance(2);
                break;
            }
            if (peek() == '\n') line_count++;
            advance();
        }
        return yylex(); // Continue to next token
    }
    
    // Handle numbers
    if (isdigit(peek())) {
        std::string number;
        while (inputPos < inputText.length() && isdigit(peek())) {
            number += peek();
            advance();
        }
        yytext = strdup(number.c_str());
        yylval.number = atoi(yytext);
        return NUMBER;
    }
    
    // Handle string literals
    if (peek() == '"') {
        advance(); // Skip opening quote
        std::string str;
        while (inputPos < inputText.length() && peek() != '"') {
            if (peek() == '\\' && inputPos + 1 < inputText.length()) {
                advance();
                char escaped = peek();
                switch (escaped) {
                    case 'n': str += '\n'; break;
                    case 't': str += '\t'; break;
                    case '"': str += '"'; break;
                    case '\\': str += '\\'; break;
                    default: str += escaped; break;
                }
                advance();
            } else {
                str += peek();
                advance();
            }
        }
        if (peek() == '"') advance(); // Skip closing quote
        
        yytext = strdup(str.c_str());
        yylval.string = strdup(str.c_str());
        return STRING;
    }
    
    // Handle keywords and identifiers
    if (isalpha(peek()) || peek() == '_') {
        std::string id;
        while (inputPos < inputText.length() && (isalnum(peek()) || peek() == '_')) {
            id += peek();
            advance();
        }
        yytext = strdup(id.c_str());
        
        // Check for keywords
        if (id == "true") return TRUE;
        if (id == "false") return FALSE;
        if (id == "print") return PRINT;
        if (id == "let") return LET;
        if (id == "in") return IN;
        if (id == "function") return FUNCTION;
        if (id == "type") return TYPE;
        if (id == "inherits") return INHERITS;
        if (id == "base") return BASE;
        if (id == "new") return NEW;
        if (id == "if") return IF;
        if (id == "elif") return ELIF;
        if (id == "else") return ELSE;
        if (id == "while") return WHILE;
        if (id == "for") return FOR;
        if (id == "is") return IS;
        if (id == "as") return AS;
        if (id == "Number") return TYPE_NUMBER;
        if (id == "String") return TYPE_STRING;
        if (id == "Boolean") return TYPE_BOOLEAN;
        
        // Otherwise it's an identifier
        yylval.string = strdup(id.c_str());
        return IDENTIFIER;
    }
    
    // Handle newlines
    if (peek() == '\n') {
        advance();
        line_count++;
        return yylex(); // Continue to next token (ignore newlines as whitespace)
    }
    
    // Handle operators and punctuation
    if (matchString("=>")) {
        yytext = strdup("=>");
        advance(2);
        return ARROW;
    }
    if (matchString(":=")) {
        yytext = strdup(":=");
        advance(2);
        return ASSIGN;
    }
    if (matchString("==")) {
        yytext = strdup("==");
        advance(2);
        return EQEQ;
    }
    if (matchString("!=")) {
        yytext = strdup("!=");
        advance(2);
        return NOTEQ;
    }
    if (matchString("<=")) {
        yytext = strdup("<=");
        advance(2);
        return LESSEQ;
    }
    if (matchString(">=")) {
        yytext = strdup(">=");
        advance(2);
        return GREATEREQ;
    }
    if (matchString("@@")) {
        yytext = strdup("@@");
        advance(2);
        return CONCAT_SPACE;
    }
    
    // Single character tokens
    char c = peek();
    advance();
    yytext = (char*)malloc(2);
    yytext[0] = c;
    yytext[1] = '\0';
    
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
            return -1;
    }
}
)";
        
        return result;
    }
    
public:
    void generate() {
        parseTokensFromParser();
        parseLexerFile();
        
        // Generate header file
        std::ofstream header("../hulk/lexer.hpp");
        if (header.is_open()) {
            header << generateHeader();
            header.close();
            std::cout << "Generated ../hulk/lexer.hpp" << std::endl;
        } else {
            std::cerr << "Error: Cannot create ../hulk/lexer.hpp" << std::endl;
        }
        
        // Generate source file
        std::ofstream source("../hulk/lexer.cpp");
        if (source.is_open()) {
            source << generateImplementation();
            source.close();
            std::cout << "Generated ../hulk/lexer.cpp" << std::endl;
        } else {
            std::cerr << "Error: Cannot create ../hulk/lexer.cpp" << std::endl;
        }
    }
};

int main() {
    SimpleLexerGenerator generator;
    generator.generate();
    return 0;
}