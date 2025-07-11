#include "AST/Program.hpp"
#include "Context/Context.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include <iostream>
#include <vector>
#include <string>

void Program::execute() const {
    for (const auto& statement : Statements) {
        if (statement) {
            statement->print();
            statement->execute();
        }
    }
}

bool Program::validate() {
    std::cout << "DEBUG: Creating context..." << std::endl;
    Context* context = new Context();
    bool valid = true;
    std::vector<std::string> allErrors;

    // First pass: register all types
    std::cout << "DEBUG: First pass - registering types..." << std::endl;
    std::vector<TypeDefNode*> types;
    
    for (size_t i = 0; i < Statements.size(); i++) {
        TypeDefNode* typeNode = dynamic_cast<TypeDefNode*>(Statements[i]);
        if (typeNode) {
            std::cout << "DEBUG: Registering type " << typeNode->typeName << std::endl;
            if (!typeNode->validate(context)) {
                allErrors.push_back("Error in statement " + std::to_string(i+1) + ": " +
                                   typeNode->getErrorMessage());
                valid = false;
                // Continue validating other types instead of breaking
            } else {
                types.push_back(typeNode);
            }
        }
    }

    // Second pass: register all function signatures
    std::cout << "DEBUG: Second pass - registering functions..." << std::endl;
    std::vector<DefFuncNode*> functions;
    
    for (size_t i = 0; i < Statements.size(); i++) {
        DefFuncNode* funcNode = dynamic_cast<DefFuncNode*>(Statements[i]);
        if (funcNode) {
            std::cout << "DEBUG: Registering function " << funcNode->identifier << std::endl;
            if (!funcNode->validate(context)) {
                allErrors.push_back("Error in statement " + std::to_string(i+1) + ": " +
                                   funcNode->getErrorMessage());
                valid = false;
                // Continue validating other functions instead of breaking
            } else {
                functions.push_back(funcNode);
            }
        }
    }

    // Third pass: validate function bodies
    std::cout << "DEBUG: Third pass - validating function bodies..." << std::endl;
    for (auto funcNode : functions) {
        std::cout << "DEBUG: Validating body of function " << funcNode->identifier << std::endl;
        if (!funcNode->validateBody(context)) {
            allErrors.push_back("Error in function '" + funcNode->identifier + "': " +
                               funcNode->getErrorMessage());
            valid = false;
            // Continue validating other function bodies
        }
    }

    // Fourth pass: validate all other statements
    std::cout << "DEBUG: Fourth pass - validating " << Statements.size() << " statements..." << std::endl;
    for (size_t i = 0; i < Statements.size(); i++) {
        // Skip functions and types as they were already validated
        if (dynamic_cast<DefFuncNode*>(Statements[i]) || dynamic_cast<TypeDefNode*>(Statements[i])) {
            continue;
        }
        
        std::cout << "DEBUG: Validating statement " << (i+1) << std::endl;
        if (!Statements[i]->validate(context)) {
            allErrors.push_back("Error in statement " + std::to_string(i+1) + ": " +
                               Statements[i]->getErrorMessage());
            valid = false;
            // Continue validating other statements
        } else {
            std::cout << "DEBUG: Statement " << (i+1) << " validated successfully" << std::endl;
        }
    }

    // Combine all errors into a single error message
    if (!allErrors.empty()) {
        errorMessage = "";
        for (size_t i = 0; i < allErrors.size(); i++) {
            if (i > 0) errorMessage += "\n";
            errorMessage += allErrors[i];
        }
    }

    std::cout << "DEBUG: Deleting context..." << std::endl;
    delete context;
    std::cout << "DEBUG: Validation complete, result: " << valid << std::endl;
    return valid;
}

void Program::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

Program::~Program() {
    for (auto statement : Statements) {
        delete statement;
    }
}