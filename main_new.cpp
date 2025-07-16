#include <iostream>
#include <fstream>
#include <sstream>
#include <AllHeaders.hpp>
#include "Globals.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h> 
#include <Visitors/LLVMCodegenVisitor.hpp>

int main(int argc, char* argv[]) {
    const char* filename = "script.hulk";
    
    if (argc > 1) {
        filename = argv[1];
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
    
    // Parse
    std::cout << "DEBUG: Starting parsing..." << std::endl;
    Parser::Parser parser;
    
    // Uncomment to see grammar information
    // parser.printGrammarInfo();
    
    Program* program = parser.parse(tokens);
    
    if (parser.hasErrors()) {
        std::cerr << "Parse errors:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return 1;
    }
    
    if (!program) {
        std::cerr << "Failed to parse program" << std::endl;
        return 1;
    }
    
    std::cout << "DEBUG: Number of statements: " << program->Statements.size() << std::endl;
    
    // Validate
    std::cout << "DEBUG: Starting validation..." << std::endl;
    if (program->validate()) {
        std::cout << "¡Programa validado correctamente!" << std::endl;

        // Generate LLVM IR
        std::cout << "DEBUG: Initializing LLVM components..." << std::endl;
        
        llvm::LLVMContext context;
        llvm::IRBuilder<> builder(context);
        llvm::Module module("hulk_module", context);
        
        LLVMCodegenVisitor codegenVisitor(context, builder, module);

        try {
            std::cout << "DEBUG: Starting code generation..." << std::endl;
            
            program->accept(codegenVisitor); 

            std::cout << "DEBUG: Writing output.ll..." << std::endl;
            
            std::error_code EC;
            llvm::raw_fd_ostream dest("output.ll", EC, llvm::sys::fs::OF_None);
            if (EC) {
                llvm::errs() << "Could not open file: " << EC.message();
                delete program;
                return 1;
            }
            module.print(dest, nullptr);
            std::cout << "LLVM IR generated to output.ll" << std::endl;
            
            delete program;
            return 0;
            
        } catch (const std::exception& e) {
            std::cerr << "Error durante la generación de código LLVM: " << e.what() << std::endl;
            delete program;
            return 1;
        }
    } else {
        std::cerr << "Error de validación del programa: " << program->getErrorMessage() << std::endl;
        delete program;
        return 1;
    }
}