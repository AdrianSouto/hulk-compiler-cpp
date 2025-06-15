#include "FlexLexerParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool FlexLexerParser::parseFlexFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Parse tokens from parser.y first
    parseTokensFromParser("parser.y");
    
    // Find sections
    size_t firstSeparator = content.find("%%");
    size_t secondSeparator = content.find("%%", firstSeparator + 2);
    
    if (firstSeparator == std::string::npos || secondSeparator == std::string::npos) {
        std::cerr << "Error: Invalid flex file format" << std::endl;
        return false;
    }
    
    // Extract header code
    std::string definitionSection = content.substr(0, firstSeparator);
    std::string rulesSection = content.substr(firstSeparator + 2, 
                                             secondSeparator - firstSeparator - 2);
    
    // Parse definition section for header code
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
    int priority = 0;
    
    while (std::getline(rulesStream, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty() || line[0] == '/') continue;
        
        // Find pattern and action
        size_t spacePos = line.find_first_of(" \t{");
        if (spacePos != std::string::npos) {
            std::string pattern = line.substr(0, spacePos);
            std::string action = line.substr(spacePos);
            
            // Clean up action
            size_t braceStart = action.find('{');
            size_t braceEnd = action.find_last_of('}');
            if (braceStart != std::string::npos && braceEnd != std::string::npos) {
                action = action.substr(braceStart + 1, braceEnd - braceStart - 1);
            }
            
            // Trim action
            action.erase(0, action.find_first_not_of(" \t\n"));
            action.erase(action.find_last_not_of(" \t\n") + 1);
            
            TokenInfo token;
            token.pattern = pattern;
            token.action = action;
            token.priority = priority++;
            
            // Extract token name from action
            if (action.find("return") != std::string::npos) {
                size_t returnPos = action.find("return");
                size_t tokenStart = action.find_first_not_of(" \t", returnPos + 6);
                size_t tokenEnd = action.find_first_of(" \t;", tokenStart);
                if (tokenEnd == std::string::npos) tokenEnd = action.length();
                
                token.name = action.substr(tokenStart, tokenEnd - tokenStart);
            } else {
                token.name = "SKIP";
            }
            
            tokens.push_back(token);
        }
    }
    
    return true;
}

void FlexLexerParser::parseTokensFromParser(const std::string& parserFile) {
    std::ifstream file(parserFile);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("%token") != std::string::npos) {
            std::istringstream tokenStream(line);
            std::string word;
            bool foundToken = false;
            
            while (tokenStream >> word) {
                if (foundToken && word.find('<') == std::string::npos) {
                    tokenMap[word] = word;
                } else if (word == "%token" || word.find("%token<") == 0) {
                    foundToken = true;
                }
            }
        }
    }
    file.close();
}

std::string FlexLexerParser::processStringLiteral(const std::string& literal) {
    if (literal.front() == '"' && literal.back() == '"') {
        std::string content = literal.substr(1, literal.length() - 2);
        
        // Handle escape sequences
        std::string processed;
        for (size_t i = 0; i < content.length(); i++) {
            if (content[i] == '\\' && i + 1 < content.length()) {
                switch (content[i + 1]) {
                    case 'n': processed += '\n'; i++; break;
                    case 't': processed += '\t'; i++; break;
                    case 'r': processed += '\r'; i++; break;
                    case '\\': processed += '\\'; i++; break;
                    case '"': processed += '"'; i++; break;
                    default: processed += content[i]; break;
                }
            } else {
                processed += content[i];
            }
        }
        return processed;
    }
    return literal;
}

std::string FlexLexerParser::generateTokenDefinitions() {
    std::string result = "// Token definitions\n";
    
    for (const auto& pair : tokenMap) {
        result += "#define " + pair.first + " " + std::to_string(std::hash<std::string>{}(pair.first) % 1000 + 256) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string FlexLexerParser::generateLexerHeader() {
    return R"(#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <iostream>

)" + headerCode + R"(

extern int line_count;
extern char* yytext;

union YYSTYPE {
    int number;
    double decimal;
    char* string;
};

extern YYSTYPE yylval;

int yylex();
void setInput(const std::string& input);

#endif // LEXER_HPP
)";
}

std::string FlexLexerParser::generateLexerImplementation() {
    std::string result = R"(#include "lexer.hpp"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <regex>

static std::string inputText;
static size_t inputPos = 0;
int line_count = 1;
char* yytext = nullptr;
YYSTYPE yylval;

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

static bool matchRegex(const std::string& pattern, std::string& matched) {
    try {
        std::regex regex(pattern);
        std::smatch match;
        std::string remaining = inputText.substr(inputPos);
        
        if (std::regex_search(remaining, match, pattern) && match.position() == 0) {
            matched = match.str();
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

int yylex() {
    // Skip whitespace
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
    
    std::string matched;
    
)";

    // Generate matching code for each token
    for (const auto& token : tokens) {
        if (token.name == "SKIP") continue;
        
        result += "    // Pattern: " + token.pattern + "\n";
        
        if (token.pattern.front() == '"' && token.pattern.back() == '"') {
            // String literal
            std::string literal = processStringLiteral(token.pattern);
            result += "    if (matchString(\"" + literal + "\")) {\n";
            result += "        yytext = strdup(\"" + literal + "\");\n";
            result += "        advance(" + std::to_string(literal.length()) + ");\n";
        } else if (token.pattern == "[0-9]+") {
            // Number pattern
            result += "    if (isdigit(peek())) {\n";
            result += "        std::string number;\n";
            result += "        while (inputPos < inputText.length() && isdigit(peek())) {\n";
            result += "            number += peek();\n";
            result += "            advance();\n";
            result += "        }\n";
            result += "        yytext = strdup(number.c_str());\n";
        } else if (token.pattern.find("\"") != std::string::npos && token.pattern.find("\\\\") != std::string::npos) {
            // String literal with escapes
            result += "    if (peek() == '\"') {\n";
            result += "        advance(); // Skip opening quote\n";
            result += "        std::string str;\n";
            result += "        while (inputPos < inputText.length() && peek() != '\"') {\n";
            result += "            if (peek() == '\\\\' && inputPos + 1 < inputText.length()) {\n";
            result += "                advance();\n";
            result += "                char escaped = peek();\n";
            result += "                switch (escaped) {\n";
            result += "                    case 'n': str += '\\n'; break;\n";
            result += "                    case 't': str += '\\t'; break;\n";
            result += "                    case '\"': str += '\"'; break;\n";
            result += "                    case '\\\\': str += '\\\\'; break;\n";
            result += "                    default: str += escaped; break;\n";
            result += "                }\n";
            result += "                advance();\n";
            result += "            } else {\n";
            result += "                str += peek();\n";
            result += "                advance();\n";
            result += "            }\n";
            result += "        }\n";
            result += "        if (peek() == '\"') advance(); // Skip closing quote\n";
            result += "        yytext = strdup(str.c_str());\n";
        } else if (token.pattern == "[a-zA-Z][a-zA-Z0-9_]*") {
            // Identifier pattern
            result += "    if (isalpha(peek()) || peek() == '_') {\n";
            result += "        std::string id;\n";
            result += "        while (inputPos < inputText.length() && (isalnum(peek()) || peek() == '_')) {\n";
            result += "            id += peek();\n";
            result += "            advance();\n";
            result += "        }\n";
            result += "        yytext = strdup(id.c_str());\n";
        } else {
            // Try to match as string literal
            std::string literal = token.pattern;
            result += "    if (matchString(\"" + literal + "\")) {\n";
            result += "        yytext = strdup(\"" + literal + "\");\n";
            result += "        advance(" + std::to_string(literal.length()) + ");\n";
        }
        
        // Handle action
        if (token.action.find("yylval.number") != std::string::npos) {
            result += "        yylval.number = atoi(yytext);\n";
        } else if (token.action.find("yylval.string") != std::string::npos) {
            result += "        yylval.string = strdup(yytext);\n";
        }
        
        if (token.action.find("line_count++") != std::string::npos) {
            result += "        line_count++;\n";
        }
        
        if (token.action.find("return") != std::string::npos) {
            result += "        return " + token.name + ";\n";
        } else {
            result += "        return yylex(); // Continue to next token\n";
        }
        
        result += "    }\n\n";
    }
    
    result += R"(
    // Handle newlines
    if (peek() == '\n') {
        advance();
        line_count++;
        return yylex(); // Continue to next token
    }
    
    // Handle unexpected characters
    char unexpected = peek();
    advance();
    yytext = (char*)malloc(2);
    yytext[0] = unexpected;
    yytext[1] = '\0';
    
    return -1; // Error token
}
)";
    
    return result;
}

bool FlexLexerParser::generateFiles(const std::string& headerFile, const std::string& sourceFile) {
    // Generate header
    std::ofstream header(headerFile);
    if (!header.is_open()) {
        std::cerr << "Error: Cannot create " << headerFile << std::endl;
        return false;
    }
    
    header << generateTokenDefinitions();
    header << generateLexerHeader();
    header.close();
    
    // Generate source
    std::ofstream source(sourceFile);
    if (!source.is_open()) {
        std::cerr << "Error: Cannot create " << sourceFile << std::endl;
        return false;
    }
    
    source << generateLexerImplementation();
    source.close();
    
    return true;
}