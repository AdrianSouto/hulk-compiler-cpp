#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "AST/StatementNode.hpp"
#include <vector>
#include <string>

class LLVMCodegenVisitor;

class Program {
public:
    std::vector<StatementNode*> Statements;
    std::string errorMessage;


    bool validate();
    const std::string& getErrorMessage() const { return errorMessage; }
    void accept(LLVMCodegenVisitor& visitor);
    ~Program();
};

#endif
