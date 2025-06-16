// Generated Bison-Compatible Lexer
// Auto-generated - do not edit manually

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

// Forward declarations and external variables
extern "C" {
    extern union YYSTYPE {
        int number;
        double decimal;
        char* string;
    } yylval;
}

// Token definitions
#define NUMBER 258
#define STRING 259
#define IDENTIFIER 260
#define PRINT 261
#define LET 263
#define IN 264
#define FUNCTION 265
#define ARROW 266
#define TYPE 267
#define INHERITS 268
#define BASE 269
#define NEW 270
#define IF 271
#define ELIF 272
#define ELSE 273
#define IS 274
#define AS 275
#define LPAREN 276
#define RPAREN 277
#define LBRACE 278
#define RBRACE 279
#define TYPE_NUMBER 280
#define TYPE_STRING 281
#define TYPE_BOOLEAN 282
#define TRUE 283
#define FALSE 284
#define WHILE 285
#define FOR 286
#define PLUS 287
#define MINUS 288
#define MULTIPLY 289
#define DIVIDE 290
#define MODULO 291
#define COLON 292
#define POWER 293
#define CONCAT 294
#define CONCAT_SPACE 295
#define SEMICOLON 296
#define COMMA 297
#define EQUALS 298
#define EQEQ 299
#define AND 300
#define GREATEREQ 301
#define NOTEQ 302
#define LESS 303
#define GREATER 304
#define OR 305
#define NOT 306
#define LESSEQ 307
#define ASSIGN 308
#define DOT 309

// Global variables expected by Bison
char* yytext = nullptr;
int yylineno = 1;
int yycolumn = 1;
std::string input_text;
size_t input_pos = 0;

// Keywords map
std::unordered_map<std::string, int> keywords = {
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
};

// Helper functions
char current_char() {
    if (input_pos >= input_text.length()) return '\0';
    return input_text[input_pos];
}

char peek_char(int offset = 1) {
    size_t pos = input_pos + offset;
    if (pos >= input_text.length()) return '\0';
    return input_text[pos];
}

void advance() {
    if (input_pos < input_text.length()) {
        if (current_char() == '\n') {
            yylineno++;
            yycolumn = 1;
        } else {
            yycolumn++;
        }
        input_pos++;
    }
}

void skip_whitespace() {
    while (current_char() != '\0' && std::isspace(current_char())) {
        advance();
    }
}

std::string read_number() {
    std::string result;
    while (current_char() != '\0' && std::isdigit(current_char())) {
        result += current_char();
        advance();
    }
    if (current_char() == '.' && std::isdigit(peek_char())) {
        result += current_char();
        advance();
        while (current_char() != '\0' && std::isdigit(current_char())) {
            result += current_char();
            advance();
        }
    }
    return result;
}

std::string read_identifier() {
    std::string result;
    while (current_char() != '\0' && (std::isalnum(current_char()) || current_char() == '_')) {
        result += current_char();
        advance();
    }
    return result;
}

std::string read_string() {
    std::string result;
    advance(); // Skip opening quote
    while (current_char() != '\0' && current_char() != '"') {
        if (current_char() == '\\' && peek_char() != '\0') {
            advance(); // Skip backslash
            switch (current_char()) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                default: result += current_char(); break;
            }
        } else {
            result += current_char();
        }
        advance();
    }
    if (current_char() == '"') advance(); // Skip closing quote
    return result;
}

// Main lexer function
int yylex() {
    skip_whitespace();
    
    if (current_char() == '\0') {
        return 0; // EOF
    }
    
    // Free previous yytext
    if (yytext) {
        free(yytext);
        yytext = nullptr;
    }
    
    // Numbers
    if (std::isdigit(current_char())) {
        std::string num = read_number();
        yytext = strdup(num.c_str());
        yylval.number = std::atoi(num.c_str());
        printf("<NUMBER:%s>\n", num.c_str());
        return NUMBER;
    }
    
    // Identifiers and keywords
    if (std::isalpha(current_char()) || current_char() == '_') {
        std::string id = read_identifier();
        yytext = strdup(id.c_str());
        
        // Check if it's a keyword
        auto it = keywords.find(id);
        if (it != keywords.end()) {
            printf("<%s>\n", id.c_str());
            return it->second;
        } else {
            yylval.string = strdup(id.c_str());
            printf("<ID:%s>\n", id.c_str());
            return IDENTIFIER;
        }
    }
    
    // Strings
    if (current_char() == '"') {
        std::string str = read_string();
        yytext = strdup(str.c_str());
        yylval.string = strdup(str.c_str());
        printf("<STRING:%s>\n", str.c_str());
        return STRING;
    }
    
    // Multi-character operators
    if (current_char() == '=' && peek_char() == '>') {
        yytext = strdup("=>");
        advance(); advance();
        return ARROW;
    }
    if (current_char() == ':' && peek_char() == '=') {
        yytext = strdup(":=");
        advance(); advance();
        return ASSIGN;
    }
    if (current_char() == '=' && peek_char() == '=') {
        yytext = strdup("==");
        advance(); advance();
        return EQEQ;
    }
    if (current_char() == '!' && peek_char() == '=') {
        yytext = strdup("!=");
        advance(); advance();
        return NOTEQ;
    }
    if (current_char() == '<' && peek_char() == '=') {
        yytext = strdup("<=");
        advance(); advance();
        return LESSEQ;
    }
    if (current_char() == '>' && peek_char() == '=') {
        yytext = strdup(">=");
        advance(); advance();
        return GREATEREQ;
    }
    if (current_char() == '@' && peek_char() == '@') {
        yytext = strdup("@@");
        advance(); advance();
        return CONCAT_SPACE;
    }
    
    // Single character tokens
    char c = current_char();
    yytext = (char*)malloc(2);
    yytext[0] = c;
    yytext[1] = '\0';
    advance();
    
    switch (c) {
        case '@':
            return CONCAT;
            break;
        case ';':
            return SEMICOLON;
            break;
        case '=':
            return EQUALS;
            break;
        case ',':
            return COMMA;
            break;
        case ':':
            return COLON;
            break;
        case '(':
            return LPAREN;
            break;
        case ')':
            return RPAREN;
            break;
        case '+':
            return PLUS;
            break;
        case '-':
            return MINUS;
            break;
        case '*':
            return MULTIPLY;
            break;
        case '/':
            return DIVIDE;
            break;
        case '%':
            return MODULO;
            break;
        case '^':
            return POWER;
            break;
        case '{':
            return LBRACE;
            break;
        case '}':
            return RBRACE;
            break;
        case '<':
            return LESS;
            break;
        case '>':
            return GREATER;
            break;
        case '&':
            return AND;
            break;
        case '|':
            return OR;
            break;
        case '!':
            return NOT;
            break;
        case '.':
            return DOT;
            break;
        default:
            printf("Error: Carácter no reconocido '%c' en línea %d, columna %d\n", c, yylineno, yycolumn);
            return c;
    }
}

void set_input_from_file(FILE* file) {
    if (!file) return;
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    input_text.resize(length);
    fread(&input_text[0], 1, length, file);
    input_pos = 0;
    yylineno = 1;
    yycolumn = 1;
}

void set_input(const std::string& text) {
    input_text = text;
    input_pos = 0;
    yylineno = 1;
    yycolumn = 1;
}
