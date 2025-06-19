#include <iostream>
#include <fstream>
#include <AllHeaders.hpp>
#include "Globals.hpp"

#include "hulk/parser.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h> 
#include <Visitors/LLVMCodegenVisitor.hpp>


extern void set_input_from_file(FILE* file);
extern int yyparse();
extern Program program;

int main(int argc, char* argv[]) {
    const char* filename = "../script.hulk";
    
    
    if (argc > 1) {
        filename = argv[1];
    }

    std::cout << "DEBUG: Opening file: " << filename << std::endl;
    FILE* file = fopen(filename, "r");
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }
    

    set_input_from_file(file);
    
    std::cout << "DEBUG: Starting parsing..." << std::endl;
    int parseResult = yyparse();
    std::cout << "DEBUG: Parse result: " << parseResult << std::endl;
    if (parseResult != 0) {
        std::cerr << "Error de análisis sintáctico. No se pudo procesar el programa." << std::endl;
        fclose(file);
        return 1;
    }

    std::cout << "DEBUG: Number of statements: " << program.Statements.size() << std::endl;
    if (!program.Statements.empty()) {
        std::cout << "DEBUG: Starting validation..." << std::endl;
        
        if (program.validate()) {
            std::cout << "¡Programa validado correctamente!" << std::endl;

            std::cout << "DEBUG: Initializing LLVM components..." << std::endl;
            
            llvm::LLVMContext context;
            llvm::IRBuilder<> builder(context);
            llvm::Module module("hulk_module", context);

            
            LLVMCodegenVisitor codegenVisitor(context, builder, module);

            try {
                std::cout << "DEBUG: Starting code generation..." << std::endl;
                
                program.accept(codegenVisitor); 

                std::cout << "DEBUG: Writing output.ll..." << std::endl;
                
                std::error_code EC;
                llvm::raw_fd_ostream dest("output.ll", EC, llvm::sys::fs::OF_None);
                if (EC) {
                    llvm::errs() << "Could not open file: " << EC.message();
                    fclose(file);
                    return 1;
                }
                module.print(dest, nullptr);
                std::cout << "LLVM IR generated to output.ll" << std::endl;

                
                fclose(file);
                return 0;
                
            } catch (const std::exception& e) {
                std::cerr << "Error durante la generación de código LLVM: " << e.what() << std::endl;
                fclose(file);
                return 1;
            }
        } else {
            std::cerr << "Error de validación del programa: " << program.getErrorMessage() << std::endl;
            fclose(file); 
            return 1;
        }
    } else {
        std::cerr << "No hay declaraciones en el programa. El archivo podría estar vacío o contener errores de sintaxis." << std::endl;
    }

    fclose(file);
    return 0;
}