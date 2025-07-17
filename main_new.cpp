#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"

int main(int argc, char* argv[]) {
    const char* filename = "script.hulk";
    bool showGrammarInfo = false;

    // Check for command line arguments
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--grammar-info" || std::string(argv[i]) == "-g") {
            showGrammarInfo = true;
        } else {
            filename = argv[i];
        }
    }

    std::cout << "DEBUG: Opening file: " << filename << std::endl;
    
    // Read file content
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();
    file.close();
    
    // Tokenize
    std::cout << "DEBUG: Starting tokenization..." << std::endl;
    Lexer lexer;
    std::vector<Token> tokens = lexer.tokenize(input);
    
    std::cout << "DEBUG: Tokens generated: " << tokens.size() << std::endl;

    // Parse
    std::cout << "DEBUG: Starting parsing..." << std::endl;
    Parser::Parser parser;
    
    // Show grammar information if requested
    if (showGrammarInfo) {
        parser.printGrammarInfo();
        std::cout << std::endl;
    }

    // Parse tokens into parse tree
    auto parseTree = parser.parse(tokens);

    if (parser.hasErrors()) {
        std::cerr << "Parse errors:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return 1;
    }
    
    if (!parseTree) {
        std::cerr << "Failed to parse program" << std::endl;
        return 1;
    }
    
    // Print parse tree (derivation tree)
    std::cout << "\n=== PARSE TREE (DERIVATION TREE) ===" << std::endl;
    parseTree->print();
    std::cout << "===================================\n" << std::endl;

    std::cout << "✓ Syntax analysis completed successfully!" << std::endl;
    std::cout << "✓ Derivation tree generated successfully!" << std::endl;

    return 0;
}

