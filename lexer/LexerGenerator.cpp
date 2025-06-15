#include "LexerGenerator.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

bool LexerGenerator::parseFlexFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Find the three sections separated by %%
    size_t firstSeparator = content.find("%%");
    if (firstSeparator == std::string::npos) {
        std::cerr << "Error: Invalid flex file format - missing first %%" << std::endl;
        return false;
    }
    
    size_t secondSeparator = content.find("%%", firstSeparator + 2);
    if (secondSeparator == std::string::npos) {
        std::cerr << "Error: Invalid flex file format - missing second %%" << std::endl;
        return false;
    }
    
    // Parse sections
    std::string definitionSection = content.substr(0, firstSeparator);
    std::string rulesSection = content.substr(firstSeparator + 2, 
                                             secondSeparator - firstSeparator - 2);
    std::string userCodeSection = content.substr(secondSeparator + 2);
    
    parseDefinitionSection(definitionSection);
    parseRulesSection(rulesSection);
    parseUserCodeSection(userCodeSection);
    
    return true;
}

void LexerGenerator::parseDefinitionSection(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    bool inHeaderCode = false;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) continue;
        
        if (line == "%{") {
            inHeaderCode = true;
            continue;
        } else if (line == "%}") {
            inHeaderCode = false;
            continue;
        }
        
        if (inHeaderCode) {
            definition.headerCode += line + "\n";
        } else if (line.substr(0, 7) == "%option") {
            definition.options.push_back(line.substr(7));
        }
    }
}

void LexerGenerator::parseRulesSection(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    std::string currentPattern;
    std::string currentAction;
    bool inAction = false;
    int braceCount = 0;
    int priority = 1000; // Higher number = lower priority
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) continue;
        
        if (!inAction) {
            // Look for pattern and action
            size_t actionStart = line.find_first_of(" \t{");
            if (actionStart != std::string::npos) {
                currentPattern = line.substr(0, actionStart);
                
                // Find the action part
                size_t bracePos = line.find('{', actionStart);
                if (bracePos != std::string::npos) {
                    inAction = true;
                    braceCount = 1;
                    currentAction = line.substr(bracePos + 1);
                    
                    // Count braces in the current line
                    for (size_t i = bracePos + 1; i < line.length(); i++) {
                        if (line[i] == '{') braceCount++;
                        else if (line[i] == '}') braceCount--;
                    }
                    
                    if (braceCount == 0) {
                        // Action complete on same line
                        currentAction = currentAction.substr(0, currentAction.find_last_of('}'));
                        
                        LexerRule rule;
                        rule.pattern = currentPattern;
                        rule.action = currentAction;
                        rule.isRegex = !isStringLiteral(currentPattern);
                        rule.priority = priority++;
                        definition.rules.push_back(rule);
                        
                        currentPattern.clear();
                        currentAction.clear();
                        inAction = false;
                    }
                }
            }
        } else {
            // Continue collecting action
            currentAction += "\n" + line;
            
            // Count braces
            for (char c : line) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
            }
            
            if (braceCount == 0) {
                // Action complete
                currentAction = currentAction.substr(0, currentAction.find_last_of('}'));
                
                LexerRule rule;
                rule.pattern = currentPattern;
                rule.action = currentAction;
                rule.isRegex = !isStringLiteral(currentPattern);
                rule.priority = priority++;
                definition.rules.push_back(rule);
                
                currentPattern.clear();
                currentAction.clear();
                inAction = false;
            }
        }
    }
}

void LexerGenerator::parseUserCodeSection(const std::string& content) {
    definition.userCode = content;
}

void LexerGenerator::extractTokensFromParser(const std::string& parserFile) {
    std::ifstream file(parserFile);
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open parser file " << parserFile << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Look for %token declarations
        if (line.find("%token") != std::string::npos) {
            std::istringstream tokenStream(line);
            std::string word;
            bool foundToken = false;
            
            while (tokenStream >> word) {
                if (foundToken) {
                    // Remove angle brackets if present (e.g., %token<number>)
                    if (word.find('<') != std::string::npos) {
                        continue; // Skip type declarations
                    }
                    tokenMap[word] = word;
                } else if (word == "%token" || word.find("%token<") == 0) {
                    foundToken = true;
                }
            }
        }
    }
    file.close();
}

std::string LexerGenerator::generateTokenEnum() {
    std::string result = "enum TokenType {\n";
    result += "    TOKEN_EOF = 0,\n";
    result += "    TOKEN_ERROR = -1,\n";
    
    for (const auto& token : tokenMap) {
        result += "    " + token.first + ",\n";
    }
    
    result += "};\n\n";
    return result;
}

std::string LexerGenerator::generateLexerClass() {
    std::string result = R"(
class Lexer {
private:
    std::string input;
    size_t position;
    size_t line;
    size_t column;
    
    struct Token {
        TokenType type;
        std::string value;
        size_t line;
        size_t column;
    };
    
    Token currentToken;
    
    bool matchPattern(const std::string& pattern, std::string& matched);
    bool matchRegex(const std::string& regex, std::string& matched);
    bool matchString(const std::string& str, std::string& matched);
    void skipWhitespace();
    char peek(size_t offset = 0);
    void advance(size_t count = 1);
    
public:
    union YYSTYPE {
        int number;
        double decimal;
        char* string;
    } yylval;
    
    char* yytext;
    int line_count;
    
    Lexer(const std::string& input);
    ~Lexer();
    
    int yylex();
    TokenType getNextToken();
    const Token& getCurrentToken() const { return currentToken; }
    
    // Flex compatibility
    void setInput(const std::string& input);
    std::string getText() const;
    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
};

extern Lexer* globalLexer;

// Global functions for Flex compatibility
int yylex();
)";
    
    return result;
}

std::string LexerGenerator::generateLexerImplementation() {
    std::string result = R"(
#include "lexer.hpp"
#include <regex>
#include <cstring>
#include <cstdlib>

Lexer* globalLexer = nullptr;

Lexer::Lexer(const std::string& input) : input(input), position(0), line(1), column(1) {
    line_count = 1;
    yytext = nullptr;
    yylval.string = nullptr;
    globalLexer = this;
}

Lexer::~Lexer() {
    if (yytext) {
        free(yytext);
    }
    if (yylval.string) {
        free(yylval.string);
    }
}

void Lexer::setInput(const std::string& newInput) {
    input = newInput;
    position = 0;
    line = 1;
    column = 1;
    line_count = 1;
}

char Lexer::peek(size_t offset) {
    if (position + offset >= input.length()) {
        return '\0';
    }
    return input[position + offset];
}

void Lexer::advance(size_t count) {
    for (size_t i = 0; i < count && position < input.length(); i++) {
        if (input[position] == '\n') {
            line++;
            line_count++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

bool Lexer::matchString(const std::string& str, std::string& matched) {
    if (position + str.length() > input.length()) {
        return false;
    }
    
    if (input.substr(position, str.length()) == str) {
        matched = str;
        return true;
    }
    return false;
}

bool Lexer::matchRegex(const std::string& regexStr, std::string& matched) {
    try {
        std::regex pattern(regexStr);
        std::smatch match;
        std::string remaining = input.substr(position);
        
        if (std::regex_search(remaining, match, pattern) && match.position() == 0) {
            matched = match.str();
            return true;
        }
    } catch (const std::regex_error& e) {
        // Fallback to string matching if regex fails
        return false;
    }
    return false;
}

bool Lexer::matchPattern(const std::string& pattern, std::string& matched) {
    if (pattern.front() == '"' && pattern.back() == '"') {
        // String literal
        std::string str = pattern.substr(1, pattern.length() - 2);
        return matchString(str, matched);
    } else {
        // Regex pattern
        std::string regexPattern = convertFlexPatternToRegex(pattern);
        return matchRegex(regexPattern, matched);
    }
}

TokenType Lexer::getNextToken() {
    // Skip whitespace and comments first
    while (position < input.length()) {
        char c = peek();
        if (c == ' ' || c == '\t') {
            advance();
            continue;
        }
        break;
    }
    
    if (position >= input.length()) {
        currentToken.type = TOKEN_EOF;
        currentToken.value = "";
        return TOKEN_EOF;
    }
    
    std::string matched;
    
)";

    // Generate rule matching code
    for (const auto& rule : definition.rules) {
        result += "    // Rule: " + rule.pattern + "\n";
        result += "    if (matchPattern(\"" + escapeString(rule.pattern) + "\", matched)) {\n";
        
        // Update yytext
        result += "        if (yytext) free(yytext);\n";
        result += "        yytext = strdup(matched.c_str());\n";
        result += "        advance(matched.length());\n";
        
        // Process the action
        std::string action = rule.action;
        
        // Replace common patterns in actions
        if (action.find("return") != std::string::npos) {
            // Extract the token name
            size_t returnPos = action.find("return");
            size_t tokenStart = action.find_first_not_of(" \t", returnPos + 6);
            size_t tokenEnd = action.find_first_of(" \t;}", tokenStart);
            if (tokenEnd == std::string::npos) tokenEnd = action.length();
            
            std::string tokenName = action.substr(tokenStart, tokenEnd - tokenStart);
            
            // Handle special cases
            if (action.find("yylval.number") != std::string::npos) {
                result += "        yylval.number = atoi(matched.c_str());\n";
            } else if (action.find("yylval.string") != std::string::npos) {
                if (action.find("strdup") != std::string::npos) {
                    result += "        yylval.string = strdup(matched.c_str());\n";
                } else {
                    result += "        yylval.string = strdup(matched.c_str());\n";
                }
            }
            
            result += "        currentToken.type = " + tokenName + ";\n";
            result += "        currentToken.value = matched;\n";
            result += "        return " + tokenName + ";\n";
        } else if (action.find("/*") != std::string::npos || action.find("ignore") != std::string::npos) {
            // Comment or ignore action - continue to next token
            result += "        continue;\n";
        } else {
            // Custom action - try to execute it
            result += "        " + action + "\n";
            result += "        continue;\n";
        }
        
        result += "    }\n\n";
    }
    
    result += R"(
    // No rule matched - return error or unexpected character
    currentToken.type = TOKEN_ERROR;
    currentToken.value = std::string(1, peek());
    advance();
    return TOKEN_ERROR;
}

int Lexer::yylex() {
    return static_cast<int>(getNextToken());
}

std::string Lexer::getText() const {
    return yytext ? std::string(yytext) : "";
}

// Global function for Flex compatibility
int yylex() {
    if (globalLexer) {
        return globalLexer->yylex();
    }
    return 0;
}
)";
    
    return result;
}

std::string LexerGenerator::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '"' || c == '\\') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

std::string LexerGenerator::convertFlexPatternToRegex(const std::string& pattern) {
    std::string result = pattern;
    
    // Convert common Flex patterns to C++ regex
    // This is a simplified conversion - a full implementation would be more complex
    
    // Replace [0-9]+ with \d+
    size_t pos = 0;
    while ((pos = result.find("[0-9]+", pos)) != std::string::npos) {
        result.replace(pos, 6, "\\d+");
        pos += 3;
    }
    
    // Replace [a-zA-Z] patterns
    pos = 0;
    while ((pos = result.find("[a-zA-Z]", pos)) != std::string::npos) {
        result.replace(pos, 9, "[a-zA-Z]");
        pos += 9;
    }
    
    // Handle string literals in quotes
    if (result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.length() - 2);
        // Escape special regex characters
        std::string escaped;
        for (char c : result) {
            if (c == '.' || c == '*' || c == '+' || c == '?' || c == '^' || c == '$' || 
                c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || 
                c == '|' || c == '\\') {
                escaped += '\\';
            }
            escaped += c;
        }
        result = escaped;
    }
    
    return result;
}

bool LexerGenerator::isStringLiteral(const std::string& pattern) {
    return pattern.front() == '"' && pattern.back() == '"';
}

bool LexerGenerator::generateLexer(const std::string& headerFile, const std::string& sourceFile) {
    // Generate header file
    std::ofstream header(headerFile);
    if (!header.is_open()) {
        std::cerr << "Error: Cannot create header file " << headerFile << std::endl;
        return false;
    }
    
    header << "#ifndef LEXER_HPP\n";
    header << "#define LEXER_HPP\n\n";
    header << "#include <string>\n";
    header << "#include <iostream>\n\n";
    
    // Include original header code
    header << definition.headerCode << "\n";
    
    // Generate token enum
    header << generateTokenEnum();
    
    // Generate lexer class
    header << generateLexerClass();
    
    header << "\n#endif // LEXER_HPP\n";
    header.close();
    
    // Generate source file
    std::ofstream source(sourceFile);
    if (!source.is_open()) {
        std::cerr << "Error: Cannot create source file " << sourceFile << std::endl;
        return false;
    }
    
    source << generateLexerImplementation();
    source.close();
    
    return true;
}