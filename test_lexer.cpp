#include "Lexer/Lexer.hpp"
#include <iostream>
#include <string>

void printTokens(const std::vector<Token>& tokens) {
    std::cout << "=== LEXER TEST RESULTS ===" << std::endl;
    std::cout << "Row\tCol\tKind\t\t\tText" << std::endl;
    std::cout << "---\t---\t----\t\t\t----" << std::endl;
    
    for (const auto& token : tokens) {
        std::cout << token.row << "\t" << token.col << "\t"
                  << tokenKindToString(token.kind) << "\t\t";
        
        if (token.kind == TOKEN_TEXT) {
            std::cout << "\"" << token.text << "\"";
        } else if (token.text.empty() && token.kind == TOKEN_ENDFILE) {
            std::cout << "<EOF>";
        } else {
            std::cout << token.text;
        }
        
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    Lexer lexer;
    
    // Test 1: Basic arithmetic expression
    std::cout << "TEST 1: Basic arithmetic expression" << std::endl;
    std::string test1 = "x := 42 + 3.14 * y";
    auto tokens1 = lexer.analyze(test1);
    printTokens(tokens1);
    
    // Test 2: String literals and keywords
    std::cout << "TEST 2: String literals and keywords" << std::endl;
    std::string test2 = "declare x : Numeric := \"Hello World\"";
    auto tokens2 = lexer.analyze(test2);
    printTokens(tokens2);
    
    // Test 3: Comparison operators
    std::cout << "TEST 3: Comparison operators" << std::endl;
    std::string test3 = "when x == 5 && y != 10 || z <= 3";
    auto tokens3 = lexer.analyze(test3);
    printTokens(tokens3);
    
    // Test 4: Function definition
    std::cout << "TEST 4: Function definition" << std::endl;
    std::string test4 = "procedure factorial(n: Numeric) => Numeric {\n    when n <= 1 => 1;\n    otherwise => n * factorial(n - 1);\n}";
    auto tokens4 = lexer.analyze(test4);
    printTokens(tokens4);
    
    // Test 5: Class definition
    std::cout << "TEST 5: Class definition" << std::endl;
    std::string test5 = "class Point {\n    x: Numeric;\n    y: Numeric;\n}";
    auto tokens5 = lexer.analyze(test5);
    printTokens(tokens5);
    
    // Test 6: Comments and whitespace
    std::cout << "TEST 6: Comments and whitespace" << std::endl;
    std::string test6 = "x := 42; // This is a comment\ny := 24;";
    auto tokens6 = lexer.analyze(test6);
    printTokens(tokens6);
    
    // Test 7: Error handling - invalid characters
    std::cout << "TEST 7: Error handling - invalid characters" << std::endl;
    std::string test7 = "x := 42 $ invalid";
    auto tokens7 = lexer.analyze(test7);
    printTokens(tokens7);
    
    return 0;
}