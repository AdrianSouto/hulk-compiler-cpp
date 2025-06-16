#ifndef LEXER_GENERATOR_HPP
#define LEXER_GENERATOR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

struct TokenRule {
    std::string pattern;
    std::string action;
    std::string token_name;
    bool ignore;
};

class FixedLexerGenerator {
private:
    std::vector<std::string> includes;
    std::vector<TokenRule> rules;
    
    std::string escapeForCpp(const std::string& str);
    std::string escapeAction(const std::string& action);
    std::string formatTokenAction(const std::string& action);
    
public:
    bool parseDefinitionFile(const std::string& filename);
    void parseRule(const std::string& line);
    void generateLexer(const std::string& output_filename);
};

#endif // LEXER_GENERATOR_HPP