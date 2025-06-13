#ifndef LEXER_GENERATOR_HPP
#define LEXER_GENERATOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

struct LexerRule {
    std::string pattern;        
    std::string action;         
    std::string token_name;     
    int priority;               
    
    LexerRule(const std::string& pat, const std::string& act, const std::string& token = "", int prio = 0)
        : pattern(pat), action(act), token_name(token), priority(prio) {}
};

struct LexerState {
    std::string name;
    std::vector<LexerRule> rules;
    
    LexerState(const std::string& n = "INITIAL") : name(n) {}
};

class LexerGenerator {
private:
    std::vector<LexerState> states;
    std::unordered_map<std::string, std::string> definitions;
    std::string header_code;
    std::string user_code;
    std::string class_name;
    
    
    std::string escapeString(const std::string& str);
    std::string generateTokenEnum();
    std::string generateKeywordMap();
    std::string generateRuleMatching();
    std::string generateHeaderFile();
    std::string generateImplementationFile();
    void parseFlexFile(const std::string& filename);
    void parseDefinitionsSection(const std::string& content);
    void parseRulesSection(const std::string& content);
    void parseUserCodeSection(const std::string& content);
    
public:
    LexerGenerator(const std::string& className = "GeneratedLexer");
    
    
    void addRule(const std::string& pattern, const std::string& action, 
                 const std::string& token_name = "", int priority = 0, 
                 const std::string& state = "INITIAL");
    
    void addDefinition(const std::string& name, const std::string& pattern);
    void setHeaderCode(const std::string& code);
    void setUserCode(const std::string& code);
    
    
    void generateFromFlexFile(const std::string& flexFile);
    
    
    void generateLexer(const std::string& headerPath, const std::string& implPath);
    
    
    void generateHulkLexer();
};

#endif 