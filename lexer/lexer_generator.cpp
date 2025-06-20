#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <regex>
#include <algorithm>
#include <set>

struct TokenRule {
    std::string pattern;
    std::string action;
    std::string token_name;
    bool ignore;
    bool is_keyword;
    bool is_identifier_pattern;
    bool is_number_pattern;
    bool is_string_pattern;
    bool is_comment_pattern;
};

class GenericLexerGenerator {
private:
    std::vector<std::string> includes;
    std::vector<TokenRule> rules;
    std::set<std::string> keywords;
    std::string header_file = "parser.hpp";
    std::string ast_headers_file = "../include/AllHeaders.hpp";
    
    std::string escapeForCpp(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\t': result += "\\t"; break;
                case '\r': result += "\\r"; break;
                case '<': result += "<"; break;
                case '>': result += ">"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    std::string escapeAction(const std::string& action) {
        std::string result;
        size_t start = 0;
        size_t quote_pos = action.find('"');
        
        while (quote_pos != std::string::npos) {

            result += action.substr(start, quote_pos - start);
            

            size_t end_quote = action.find('"', quote_pos + 1);
            if (end_quote == std::string::npos) {

                result += action.substr(quote_pos);
                break;
            }
            

            std::string quoted = action.substr(quote_pos + 1, end_quote - quote_pos - 1);

            result += "\"" + escapeForCpp(quoted) + "\"";
            
            start = end_quote + 1;
            quote_pos = action.find('"', start);
        }
        

        if (start < action.length()) {
            result += action.substr(start);
        }
        
        return result;
    }
    
    std::string formatTokenAction(const std::string& action) {
        std::string result = action;

        result.erase(0, result.find_first_not_of(" \t\n\r"));

        result.erase(result.find_last_not_of(" \t\n\r") + 1);
        

        size_t printf_pos = result.find("printf");
        if (printf_pos != std::string::npos) {
            size_t open_quote = result.find("\"", printf_pos);
            size_t close_quote = result.find("\"", open_quote + 1);
            if (open_quote != std::string::npos && close_quote != std::string::npos) {

                std::string before = result.substr(0, open_quote + 1);
                std::string format = result.substr(open_quote + 1, close_quote - open_quote - 1);
                std::string after = result.substr(close_quote);
                

                size_t pos = 0;
                while ((pos = format.find("\\n", pos)) != std::string::npos) {
                    format.replace(pos, 2, "\\\\n");
                    pos += 2;
                }
                
                result = before + format + after;
            }
        }
        

        if (!result.empty() && result.back() != ';') {
            result += ";";
        }
        
        return result;
    }
    
public:
    void setHeaderFile(const std::string& file) {
        header_file = file;
    }
    
    void setASTHeadersFile(const std::string& file) {
        ast_headers_file = file;
    }
    
    bool parseDefinitionFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return false;
        }
        
        std::string line;
        std::string section = "";
        
        while (std::getline(file, line)) {

            if (line.empty() || line[0] == '#') continue;
            

            if (line == "%{") {
                section = "includes";
                continue;
            } else if (line == "%}") {
                section = "";
                continue;
            } else if (line == "%%") {
                section = "rules";
                continue;
            }
            

            if (section == "includes") {
                includes.push_back(line);
            } else if (section == "rules") {
                parseRule(line);
            }
        }
        
        return true;
    }
    
    void parseRule(const std::string& line) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') return;
        
        // Find the action block
        size_t brace_start = std::string::npos;
        bool in_quotes = false;
        for (size_t i = 0; i < line.length(); ++i) {
            char ch = line[i];
            if (ch == '"') {
                in_quotes = !in_quotes;
            } else if (ch == '{' && !in_quotes) {
                brace_start = i;
                break;
            }
        }
        
        size_t brace_end = std::string::npos;
        in_quotes = false;
        for (size_t i = line.length(); i-- > 0;) {
            char ch = line[i];
            if (ch == '"') {
                in_quotes = !in_quotes;
            } else if (ch == '}' && !in_quotes) {
                brace_end = i;
                break;
            }
        }
        
        if (brace_start == std::string::npos || brace_end == std::string::npos || brace_end <= brace_start) {
            return;
        }
        
        std::string pattern = line.substr(0, brace_start);
        std::string action = line.substr(brace_start + 1, brace_end - brace_start - 1);
        
        // Trim whitespace
        pattern.erase(0, pattern.find_first_not_of(" \t"));
        pattern.erase(pattern.find_last_not_of(" \t") + 1);
        action.erase(0, action.find_first_not_of(" \t"));
        action.erase(action.find_last_not_of(" \t") + 1);
        
        // Skip if pattern is just braces
        if (pattern.length() > 2 && pattern.front() == '{' && pattern.back() == '}') {
            return;
        }
        
        TokenRule rule;
        rule.pattern = pattern;
        rule.action = action;
        rule.ignore = false;
        rule.is_keyword = false;
        rule.is_identifier_pattern = false;
        rule.is_number_pattern = false;
        rule.is_string_pattern = false;
        rule.is_comment_pattern = false;
        
        // Detect pattern types
        if (pattern.find("[a-zA-Z_][a-zA-Z0-9_]*") != std::string::npos) {
            rule.is_identifier_pattern = true;
        } else if (pattern.find("[0-9]") != std::string::npos) {
            rule.is_number_pattern = true;
        } else if (pattern == "\\\"[^\\\"]*\\\"") {
            rule.is_string_pattern = true;
        } else if (pattern.find("//") == 0 || pattern.find("/*") == 0) {
            rule.is_comment_pattern = true;
            rule.ignore = true;
        } else if (pattern.length() >= 2 && pattern.front() == '"' && pattern.back() == '"') {
            // It's a literal string pattern (keyword or operator)
            pattern = pattern.substr(1, pattern.length() - 2);
            rule.pattern = pattern;
            
            // Check if it's a keyword by looking at the action
            if (action.find("return") != std::string::npos && 
                std::isalpha(pattern[0])) {
                rule.is_keyword = true;
                keywords.insert(pattern);
            }
        }
        
        // Skip ignore patterns
        if (action.find("ignore") != std::string::npos || 
            action.find("/*") != std::string::npos ||
            action.empty()) {
            rule.ignore = true;
        }
        
        // Extract token name from return statement
        size_t return_pos = action.find("return ");
        if (return_pos != std::string::npos) {
            std::string return_part = action.substr(return_pos + 7);
            size_t semicolon = return_part.find(';');
            if (semicolon != std::string::npos) {
                return_part = return_part.substr(0, semicolon);
            }
            return_part.erase(0, return_part.find_first_not_of(" \t"));
            return_part.erase(return_part.find_last_not_of(" \t") + 1);
            rule.token_name = return_part;
        }
        
        rules.push_back(rule);
    }
    
    void generateLexer(const std::string& output_filename) {
        std::ofstream out(output_filename);
        

        out << "// Generated Bison-Compatible Lexer\n";
        out << "// Auto-generated - do not edit manually\n\n";
        
        out << "#include <iostream>\n";
        out << "#include <fstream>\n";
        out << "#include <string>\n";
        out << "#include <vector>\n";
        out << "#include <cctype>\n";
        out << "#include <cstdlib>\n";
        out << "#include <cstring>\n";
        out << "#include <unordered_map>\n\n";
        
        out << "// Include necessary headers for AST types\n";
        if (!ast_headers_file.empty()) {
            out << "#include \"" << ast_headers_file << "\"\n";
        }
        if (!header_file.empty()) {
            out << "#include \"" << header_file << "\"\n";
        }
        out << "\n";
        

        for (const auto& include : includes) {
            out << include << "\n";
        }
        

        out << "// Global variables expected by Bison\n";
        out << "char* yytext = nullptr;\n";
        out << "int yylineno = 1;\n";
        out << "int yycolumn = 1;\n";
        out << "std::string input_text;\n";
        out << "size_t input_pos = 0;\n\n";
        

        out << "// Keywords map\n";
        out << "std::unordered_map<std::string, int> keywords = {\n";
        for (const auto& rule : rules) {
            if (rule.is_keyword && !rule.token_name.empty()) {
                out << "    {\"" << rule.pattern << "\", " << rule.token_name << "},\n";
            }
        }
        out << "};\n\n";
        

        out << "// Helper functions\n";
        out << "char current_char() {\n";
        out << "    if (input_pos >= input_text.length()) return '\\0';\n";
        out << "    return input_text[input_pos];\n";
        out << "}\n\n";
        
        out << "char peek_char(int offset = 1) {\n";
        out << "    size_t pos = input_pos + offset;\n";
        out << "    if (pos >= input_text.length()) return '\\0';\n";
        out << "    return input_text[pos];\n";
        out << "}\n\n";
        
        out << "void advance() {\n";
        out << "    if (input_pos < input_text.length()) {\n";
        out << "        if (current_char() == '\\n') {\n";
        out << "            yylineno++;\n";
        out << "            yycolumn = 1;\n";
        out << "        } else {\n";
        out << "            yycolumn++;\n";
        out << "        }\n";
        out << "        input_pos++;\n";
        out << "    }\n";
        out << "}\n\n";
        
        out << "void skip_whitespace() {\n";
        out << "    while (current_char() != '\\0' && std::isspace(current_char())) {\n";
        out << "        advance();\n";
        out << "    }\n";
        out << "}\n\n";
        
        out << "std::string read_number() {\n";
        out << "    std::string result;\n";
        out << "    while (current_char() != '\\0' && std::isdigit(current_char())) {\n";
        out << "        result += current_char();\n";
        out << "        advance();\n";
        out << "    }\n";
        out << "    if (current_char() == '.' && std::isdigit(peek_char())) {\n";
        out << "        result += current_char();\n";
        out << "        advance();\n";
        out << "        while (current_char() != '\\0' && std::isdigit(current_char())) {\n";
        out << "            result += current_char();\n";
        out << "            advance();\n";
        out << "        }\n";
        out << "    }\n";
        out << "    return result;\n";
        out << "}\n\n";
        
        out << "std::string read_identifier() {\n";
        out << "    std::string result;\n";
        out << "    while (current_char() != '\\0' && (std::isalnum(current_char()) || current_char() == '_')) {\n";
        out << "        result += current_char();\n";
        out << "        advance();\n";
        out << "    }\n";
        out << "    return result;\n";
        out << "}\n\n";
        
        out << "std::string read_string() {\n";
        out << "    std::string result;\n";
        out << "    int start_line = yylineno;\n";
        out << "    int start_column = yycolumn;\n";
        out << "    advance(); // Skip opening quote\n";
        out << "    while (current_char() != '\\0' && current_char() != '\"') {\n";
        out << "        if (current_char() == '\\n') {\n";
        out << "            // Error: unterminated string literal\n";
        out << "            printf(\"Error: String sin terminar en línea %d, columna %d\\n\", start_line, start_column);\n";
        out << "            return result;\n";
        out << "        }\n";
        out << "        if (current_char() == '\\\\' && peek_char() != '\\0') {\n";
        out << "            advance(); // Skip backslash\n";
        out << "            switch (current_char()) {\n";
        out << "                case 'n': result += '\\n'; break;\n";
        out << "                case 't': result += '\\t'; break;\n";
        out << "                case 'r': result += '\\r'; break;\n";
        out << "                case '\\\\': result += '\\\\'; break;\n";
        out << "                case '\"': result += '\"'; break;\n";
        out << "                default: result += current_char(); break;\n";
        out << "            }\n";
        out << "        } else {\n";
        out << "            result += current_char();\n";
        out << "        }\n";
        out << "        advance();\n";
        out << "    }\n";
        out << "    if (current_char() == '\\0') {\n";
        out << "        // Error: EOF in string\n";
        out << "        printf(\"Error: Fin de archivo encontrado dentro de string en línea %d\\n\", start_line);\n";
        out << "    } else if (current_char() == '\"') {\n";
        out << "        advance(); // Skip closing quote\n";
        out << "    }\n";
        out << "    return result;\n";
        out << "}\n\n";
        

        out << "// Main lexer function\n";
        out << "int yylex() {\n";
        out << "    skip_whitespace();\n";
        out << "    \n";
        out << "    if (current_char() == '\\0') {\n";
        out << "        return 0; // EOF\n";
        out << "    }\n";
        out << "    \n";
        out << "    // Free previous yytext\n";
        out << "    if (yytext) {\n";
        out << "        free(yytext);\n";
        out << "        yytext = nullptr;\n";
        out << "    }\n";
        out << "    \n";
        out << "    // Numbers\n";
        out << "    if (std::isdigit(current_char())) {\n";
        out << "        std::string num = read_number();\n";
        out << "        yytext = strdup(num.c_str());\n";
        out << "        if (num.find('.') != std::string::npos) {\n";
        out << "            yylval.decimal = std::atof(num.c_str());\n";
        out << "        } else {\n";
        out << "            yylval.number = std::atoi(num.c_str());\n";
        out << "        }\n";
        out << "        printf(\"<NUMBER:%s>\\n\", num.c_str());\n";
        out << "        return NUMBER;\n";
        out << "    }\n";
        out << "    \n";
        out << "    // Identifiers and keywords\n";
        out << "    if (std::isalpha(current_char()) || current_char() == '_') {\n";
        out << "        std::string id = read_identifier();\n";
        out << "        yytext = strdup(id.c_str());\n";
        out << "        \n";
        out << "        // Check if it's a keyword\n";
        out << "        auto it = keywords.find(id);\n";
        out << "        if (it != keywords.end()) {\n";
        out << "            printf(\"<%s>\\n\", id.c_str());\n";
        out << "            return it->second;\n";
        out << "        } else {\n";
        out << "            yylval.string = strdup(id.c_str());\n";
        out << "            printf(\"<ID:%s>\\n\", id.c_str());\n";
        out << "            return IDENTIFIER;\n";
        out << "        }\n";
        out << "    }\n";
        out << "    \n";
        out << "    // Strings\n";
        out << "    if (current_char() == '\"') {\n";
        out << "        std::string str = read_string();\n";
        out << "        yytext = strdup(str.c_str());\n";
        out << "        yylval.string = strdup(str.c_str());\n";
        out << "        printf(\"<STRING:%s>\\n\", str.c_str());\n";
        out << "        return STRING;\n";
        out << "    }\n";
        out << "    \n";
        out << "    // Comments\n";
        out << "    if (current_char() == '/' && peek_char() == '/') {\n";
        out << "        // Skip single-line comment\n";
        out << "        advance(); advance(); // Skip //\n";
        out << "        while (current_char() != '\\0' && current_char() != '\\n') {\n";
        out << "            advance();\n";
        out << "        }\n";
        out << "        return yylex(); // Recursively call to get next token\n";
        out << "    }\n";
        out << "    \n";
        

        out << "    // Multi-character operators\n";
        // Sort rules by pattern length (longest first) to handle overlapping patterns correctly
        std::vector<TokenRule> sorted_rules = rules;
        std::sort(sorted_rules.begin(), sorted_rules.end(), 
            [](const TokenRule& a, const TokenRule& b) {
                return a.pattern.length() > b.pattern.length();
            });
        
        for (const auto& rule : sorted_rules) {
            if (rule.pattern.length() >= 2 && !rule.is_keyword && !rule.ignore) {
                out << "    if (";
                for (size_t i = 0; i < rule.pattern.length(); ++i) {
                    if (i > 0) out << " && ";
                    if (i == 0) {
                        out << "current_char() == '" << rule.pattern[i] << "'";
                    } else {
                        out << "peek_char(" << i << ") == '" << rule.pattern[i] << "'";
                    }
                }
                out << ") {\n";
                out << "        yytext = strdup(\"" << escapeForCpp(rule.pattern) << "\");\n";
                for (size_t i = 0; i < rule.pattern.length(); ++i) {
                    out << "        advance();\n";
                }
                out << "        " << escapeAction(rule.action) << "\n";
                out << "    }\n";
            }
        }
        

        out << "    \n";
        out << "    // Single character tokens\n";
        out << "    char c = current_char();\n";
        out << "    yytext = (char*)malloc(2);\n";
        out << "    yytext[0] = c;\n";
        out << "    yytext[1] = '\\0';\n";
        out << "    advance();\n";
        out << "    \n";
        out << "    switch (c) {\n";
        
        for (const auto& rule : rules) {
            if (rule.pattern.length() == 1) {
                out << "        case '" << rule.pattern[0] << "':\n";
                std::string formatted_action = formatTokenAction(rule.action);
                out << "            " << formatted_action << "\n";
                out << "            break;\n";
            }
        }
        
        out << "        default:\n";
        out << "            printf(\"Error: Carácter no reconocido '%c' en línea %d, columna %d\\n\", c, yylineno, yycolumn);\n";
        out << "            return c;\n";
        out << "    }\n";
        out << "}\n\n";
        

        out << "void set_input_from_file(FILE* file) {\n";
        out << "    if (!file) return;\n";
        out << "    fseek(file, 0, SEEK_END);\n";
        out << "    long length = ftell(file);\n";
        out << "    fseek(file, 0, SEEK_SET);\n";
        out << "    input_text.resize(length);\n";
        out << "    fread(&input_text[0], 1, length, file);\n";
        out << "    input_pos = 0;\n";
        out << "    yylineno = 1;\n";
        out << "    yycolumn = 1;\n";
        out << "}\n\n";
        
        out << "void set_input(const std::string& text) {\n";
        out << "    input_text = text;\n";
        out << "    input_pos = 0;\n";
        out << "    yylineno = 1;\n";
        out << "    yycolumn = 1;\n";
        out << "}\n";
        
        out.close();
        std::cout << "Lexer generado exitosamente en: " << output_filename << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_definicion.txt> <archivo_salida.cpp> [opciones]" << std::endl;
        std::cerr << "Opciones:" << std::endl;
        std::cerr << "  --header <archivo>     Especifica el archivo de header del parser (default: parser.hpp)" << std::endl;
        std::cerr << "  --ast-headers <archivo> Especifica el archivo de headers AST (default: ../include/AllHeaders.hpp)" << std::endl;
        return 1;
    }
    
    GenericLexerGenerator generator;
    
    // Parse optional arguments
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--header" && i + 1 < argc) {
            generator.setHeaderFile(argv[++i]);
        } else if (arg == "--ast-headers" && i + 1 < argc) {
            generator.setASTHeadersFile(argv[++i]);
        }
    }
    
    if (!generator.parseDefinitionFile(argv[1])) {
        return 1;
    }
    
    generator.generateLexer(argv[2]);
    
    return 0;
}