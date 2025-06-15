#include "LexerGenerator.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input.l> <output.hpp> <output.cpp>" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string headerFile = argv[2];
    std::string sourceFile = argv[3];
    
    LexerGenerator generator;
    
    // Extract tokens from parser.y if it exists
    generator.extractTokensFromParser("parser.y");
    
    // Parse the flex file
    if (!generator.parseFlexFile(inputFile)) {
        std::cerr << "Error: Failed to parse input file " << inputFile << std::endl;
        return 1;
    }
    
    // Generate the lexer
    if (!generator.generateLexer(headerFile, sourceFile)) {
        std::cerr << "Error: Failed to generate lexer files" << std::endl;
        return 1;
    }
    
    std::cout << "Lexer generated successfully!" << std::endl;
    std::cout << "Header: " << headerFile << std::endl;
    std::cout << "Source: " << sourceFile << std::endl;
    
    return 0;
}