#ifndef FLEX_LEXER_PARSER_HPP
#define FLEX_LEXER_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <regex>

struct TokenInfo {
    std::string name;
    std::string pattern;
    std::string action;
    int priority;
};

class FlexLexerParser {
private:
    std::vector<TokenInfo> tokens;
    std::string headerCode;
    std::map<std::string, std::string> tokenMap;
    
    void parseTokensFromParser(const std::string& parserFile);
    std::string processStringLiteral(const std::string& literal);
    std::string generateTokenDefinitions();
    std::string generateLexerHeader();
    std::string generateLexerImplementation();
    
public:
    bool parseFlexFile(const std::string& filename);
    bool generateFiles(const std::string& headerFile, const std::string& sourceFile);
};

#endif // FLEX_LEXER_PARSER_HPP