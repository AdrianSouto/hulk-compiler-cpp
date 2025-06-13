#include "AST/Program.hpp"
#include "Context/Context.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <iostream>

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

    std::cout << "DEBUG: Validating " << Statements.size() << " statements..." << std::endl;
    for (size_t i = 0; i < Statements.size(); i++) {
        std::cout << "DEBUG: Validating statement " << (i+1) << std::endl;
        if (!Statements[i]->validate(context)) {
            errorMessage = "Error in statement " + std::to_string(i+1) + ": " +
                          Statements[i]->getErrorMessage();
            valid = false;
            break;
        }
        std::cout << "DEBUG: Statement " << (i+1) << " validated successfully" << std::endl;
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