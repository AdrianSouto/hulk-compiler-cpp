#ifndef LEXER_GENERATOR_HPP
#define LEXER_GENERATOR_HPP

#include <string>
#include <vector>
#include <map>
#include <regex>

struct LexerRule {
    std::string pattern;
    std::string action;
    bool isRegex;
    int priority;
};

struct LexerDefinition {
    std::string headerCode;
    std::vector<std::string> options;
    std::vector<LexerRule> rules;
    std::string userCode;
};

class LexerGenerator {
private:
    LexerDefinition definition;
    std::map<std::string, std::string> tokenMap;
    
    void parseDefinitionSection(const std::string& content);
    void parseRulesSection(const std::string& content);
    void parseUserCodeSection(const std::string& content);
    
    std::string generateTokenEnum();
    std::string generateLexerClass();
    std::string generateLexerImplementation();
    
    std::string escapeString(const std::string& str);
    std::string convertFlexPatternToRegex(const std::string& pattern);
    bool isStringLiteral(const std::string& pattern);
    
public:
    bool parseFlexFile(const std::string& filename);
    bool generateLexer(const std::string& headerFile, const std::string& sourceFile);
    void extractTokensFromParser(const std::string& parserFile);
};

#endif // LEXER_GENERATOR_HPP