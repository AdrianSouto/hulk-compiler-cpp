#include "../../include/Lexer/LexerGenerator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

LexerGenerator::LexerGenerator(const std::string& className) : class_name(className) {
    states.emplace_back("INITIAL");
}

void LexerGenerator::addRule(const std::string& pattern, const std::string& action, 
                            const std::string& token_name, int priority, 
                            const std::string& state) {

    auto it = std::find_if(states.begin(), states.end(), 
                          [&state](const LexerState& s) { return s.name == state; });
    
    if (it == states.end()) {
        states.emplace_back(state);
        it = states.end() - 1;
    }
    
    it->rules.emplace_back(pattern, action, token_name, priority);
}

void LexerGenerator::addDefinition(const std::string& name, const std::string& pattern) {
    definitions[name] = pattern;
}

void LexerGenerator::setHeaderCode(const std::string& code) {
    header_code = code;
}

void LexerGenerator::setUserCode(const std::string& code) {
    user_code = code;
}

std::string LexerGenerator::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            default: result += c; break;
        }
    }
    return result;
}

void LexerGenerator::generateHulkLexer() {

    states.clear();
    states.emplace_back("INITIAL");
    definitions.clear();

    header_code = R"(#include "../include/AllHeaders.hpp"
#include "parser.hpp"
#include <cstdio>
#include <cstring>
#include <cctype>
#include <stdexcept>

extern int line_count;
)";


    addRule(R"(
    addRule(R"(/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)", "/* Ignore multi-line comments */", "", 1);
    

    addRule(R"([0-9]+)", R"({
        yylval.number = std::stoi(matched_text);
        return NUMBER;
    })", "NUMBER", 10);
    

    addRule(R"("(\\.|[^"\\])*")", R"({
        
        std::string str = matched_text.substr(1, matched_text.length() - 2); 
        
        
        std::string processed;
        for (size_t i = 0; i < str.length(); i++) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                switch (str[i + 1]) {
                    case 'n': processed += '\n'; i++; break;
                    case 't': processed += '\t'; i++; break;
                    case '"': processed += '"'; i++; break;
                    case '\\': processed += '\\'; i++; break;
                    default: processed += str[i]; break;
                }
            } else {
                processed += str[i];
            }
        }
        
        yylval.string = strdup(processed.c_str());
        return STRING;
    })", "STRING", 10);
    
    
    addRule("true", "return TRUE;", "TRUE", 20);
    addRule("false", "return FALSE;", "FALSE", 20);
    addRule("print", "return PRINT;", "PRINT", 20);
    addRule("let", "return LET;", "LET", 20);
    addRule("in", "return IN;", "IN", 20);
    addRule("function", "return FUNCTION;", "FUNCTION", 20);
    addRule("type", "return TYPE;", "TYPE", 20);
    addRule("inherits", "return INHERITS;", "INHERITS", 20);
    addRule("base", "return BASE;", "BASE", 20);
    addRule("new", "return NEW;", "NEW", 20);
    addRule("if", "return IF;", "IF", 20);
    addRule("elif", "return ELIF;", "ELIF", 20);
    addRule("else", "return ELSE;", "ELSE", 20);
    addRule("while", "return WHILE;", "WHILE", 20);
    addRule("for", "return FOR;", "FOR", 20);
    addRule("is", "return IS;", "IS", 20);
    addRule("as", "return AS;", "AS", 20);
    
    
    addRule("Number", "return TYPE_NUMBER;", "TYPE_NUMBER", 20);
    addRule("String", "return TYPE_STRING;", "TYPE_STRING", 20);
    addRule("Boolean", "return TYPE_BOOLEAN;", "TYPE_BOOLEAN", 20);
    
    
    addRule(R"([a-zA-Z][a-zA-Z0-9_]*)", R"({
        yylval.string = strdup(matched_text.c_str());
        return IDENTIFIER;
    })", "IDENTIFIER", 30);
    
    
    addRule("=>", "return ARROW;", "ARROW", 15);
    addRule(":=", "return ASSIGN;", "ASSIGN", 15);
    addRule("==", "return EQEQ;", "EQEQ", 15);
    addRule("!=", "return NOTEQ;", "NOTEQ", 15);
    addRule("<=", "return LESSEQ;", "LESSEQ", 15);
    addRule(">=", "return GREATEREQ;", "GREATEREQ", 15);
    addRule("@@", "return CONCAT_SPACE;", "CONCAT_SPACE", 15);
    
    
    addRule(R"(\n)", "line_count++; /* ignore newlines as whitespace */", "", 40);
    addRule(R"([ \t])", "/* ignore whitespace */", "", 40);
    addRule(";", "return SEMICOLON;", "SEMICOLON", 50);
    addRule("=", "return EQUALS;", "EQUALS", 50);
    addRule(",", "return COMMA;", "COMMA", 50);
    addRule(":", "return COLON;", "COLON", 50);
    addRule(R"(\()", "return LPAREN;", "LPAREN", 50);
    addRule(R"(\))", "return RPAREN;", "RPAREN", 50);
    addRule(R"(\+)", "return PLUS;", "PLUS", 50);
    addRule("-", "return MINUS;", "MINUS", 50);
    addRule(R"(\*)", "return MULTIPLY;", "MULTIPLY", 50);
    addRule("/", "return DIVIDE;", "DIVIDE", 50);
    addRule("%", "return MODULO;", "MODULO", 50);
    addRule(R"(\^)", "return POWER;", "POWER", 50);
    addRule("@", "return CONCAT;", "CONCAT", 50);
    addRule(R"(\{)", "return LBRACE;", "LBRACE", 50);
    addRule(R"(\})", "return RBRACE;", "RBRACE", 50);
    addRule("<", "return LESS;", "LESS", 50);
    addRule(">", "return GREATER;", "GREATER", 50);
    addRule("&", "return AND;", "AND", 50);
    addRule(R"(\|)", "return OR;", "OR", 50);
    addRule("!", "return NOT;", "NOT", 50);
    addRule(R"(\.)", "return DOT;", "DOT", 50);
    
    
    addRule(".", R"({
        printf("Carácter inesperado: '%s' en línea %d\n", matched_text.c_str(), line_count);
    })", "", 100);
}

std::string LexerGenerator::generateTokenEnum() {
    std::stringstream ss;
    ss << "
    ss << "enum TokenType {\n";
    
    
    std::vector<std::string> tokens;
    for (const auto& state : states) {
        for (const auto& rule : state.rules) {
            if (!rule.token_name.empty()) {
                if (std::find(tokens.begin(), tokens.end(), rule.token_name) == tokens.end()) {
                    tokens.push_back(rule.token_name);
                }
            }
        }
    }
    
    for (const auto& token : tokens) {
        ss << "    " << token << ",\n";
    }
    
    ss << "    EOF_TOKEN,\n";
    ss << "    UNKNOWN\n";
    ss << "};\n\n";
    
    return ss.str();
}

std::string LexerGenerator::generateRuleMatching() {
    std::stringstream ss;
    
    
    std::vector<std::pair<LexerRule, std::string>> all_rules;
    for (const auto& state : states) {
        for (const auto& rule : state.rules) {
            all_rules.emplace_back(rule, state.name);
        }
    }
    
    std::sort(all_rules.begin(), all_rules.end(), 
              [](const auto& a, const auto& b) { 
                  return a.first.priority < b.first.priority; 
              });
    
    ss << "int yylex() {\n";
    ss << "    static std::string input;\n";
    ss << "    static size_t position = 0;\n";
    ss << "    static bool initialized = false;\n";
    ss << "    \n";
    ss << "    if (!initialized) {\n";
    ss << "        
    ss << "        std::string line;\n";
    ss << "        while (std::getline(std::cin, line)) {\n";
    ss << "            input += line + \"\\n\";\n";
    ss << "        }\n";
    ss << "        initialized = true;\n";
    ss << "    }\n";
    ss << "    \n";
    ss << "    while (position < input.length()) {\n";
    ss << "        std::string remaining = input.substr(position);\n";
    ss << "        std::string matched_text;\n";
    ss << "        bool matched = false;\n";
    ss << "        \n";
    
    
    for (const auto& rule_pair : all_rules) {
        const auto& rule = rule_pair.first;
        ss << "        
        ss << "        {\n";
        ss << "            std::regex pattern(R\"(" << rule.pattern << ")\");\n";
        ss << "            std::smatch match;\n";
        ss << "            if (std::regex_search(remaining, match, pattern) && match.position() == 0) {\n";
        ss << "                matched_text = match.str();\n";
        ss << "                position += matched_text.length();\n";
        ss << "                matched = true;\n";
        ss << "                \n";
        ss << "                " << rule.action << "\n";
        ss << "                \n";
        ss << "                if (matched) break;\n";
        ss << "            }\n";
        ss << "        }\n";
        ss << "        \n";
    }
    
    ss << "        if (!matched) {\n";
    ss << "            position++; 
    ss << "        }\n";
    ss << "    }\n";
    ss << "    \n";
    ss << "    return 0; 
    ss << "}\n\n";
    
    return ss.str();
}

std::string LexerGenerator::generateHeaderFile() {
    std::stringstream ss;
    
    ss << "#ifndef " << class_name << "_HPP\n";
    ss << "#define " << class_name << "_HPP\n\n";
    
    ss << header_code << "\n\n";
    
    ss << generateTokenEnum();
    
    ss << "
    ss << "int yylex();\n";
    ss << "extern union {\n";
    ss << "    int number;\n";
    ss << "    char* string;\n";
    ss << "} yylval;\n\n";
    
    ss << "#endif 
    
    return ss.str();
}

std::string LexerGenerator::generateImplementationFile() {
    std::stringstream ss;
    
    ss << "#include \"" << class_name << ".hpp\"\n";
    ss << "#include <regex>\n";
    ss << "#include <iostream>\n";
    ss << "#include <string>\n\n";
    
    ss << "union {\n";
    ss << "    int number;\n";
    ss << "    char* string;\n";
    ss << "} yylval;\n\n";
    
    ss << generateRuleMatching();
    
    if (!user_code.empty()) {
        ss << "
        ss << user_code << "\n";
    }
    
    return ss.str();
}

void LexerGenerator::generateLexer(const std::string& headerPath, const std::string& implPath) {
    
    std::ofstream headerFile(headerPath);
    if (!headerFile.is_open()) {
        throw std::runtime_error("Cannot open header file: " + headerPath);
    }
    headerFile << generateHeaderFile();
    headerFile.close();
    
    
    std::ofstream implFile(implPath);
    if (!implFile.is_open()) {
        throw std::runtime_error("Cannot open implementation file: " + implPath);
    }
    implFile << generateImplementationFile();
    implFile.close();
    
    std::cout << "Generated lexer files:\n";
    std::cout << "  Header: " << headerPath << "\n";
    std::cout << "  Implementation: " << implPath << "\n";
}