#include "../include/Lexer/LexerGenerator.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    try {
        std::cout << "Hulk Lexer Generator\n";
        std::cout << "====================\n\n";
        
        
        std::filesystem::create_directories("generated");
        
        
        LexerGenerator generator("HulkNativeLexer");
        
        
        generator.generateHulkLexer();
        
        
        std::string headerPath = "generated/HulkNativeLexer.hpp";
        std::string implPath = "generated/HulkNativeLexer.cpp";
        
        generator.generateLexer(headerPath, implPath);
        
        std::cout << "\nLexer generation completed successfully!\n";
        std::cout << "\nTo use the native lexer:\n";
        std::cout << "1. Include the generated files in your project\n";
        std::cout << "2. Replace flex-generated lexer with the native implementation\n";
        std::cout << "3. Update your Makefile to compile the native lexer instead of using flex\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}