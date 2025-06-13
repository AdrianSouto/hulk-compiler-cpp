#include "Expressions/FuncCallNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include <iostream>
#include <cmath>

FuncCallNode::FuncCallNode(const std::string& id, const std::vector<ExpressionNode*>& arguments)
    : identifier(id), args(arguments) {}

int FuncCallNode::evaluate() const {
    
    if (identifier == "sin" || identifier == "cos" || identifier == "sqrt") {
        if (args.size() != 1) {
            std::cerr << "Error: Function '" << identifier << "' expects 1 argument, got " << args.size() << std::endl;
            return 0;
        }
        
        
        int argValue = args[0]->evaluate();
        double argDouble = static_cast<double>(argValue);
        
        
        double result;
        if (identifier == "sin") {
            result = std::sin(argDouble);
        } else if (identifier == "cos") { 
            result = std::cos(argDouble);
        } else { 
            if (argDouble < 0) {
                std::cerr << "Error: sqrt of negative number" << std::endl;
                return 0;
            }
            result = std::sqrt(argDouble);
        }
        
        
        
        if (identifier == "sqrt") {
            return static_cast<int>(std::round(result)); 
        } else {
            return static_cast<int>(std::round(result * 1000000)); 
        }
    }
    
    
    if (functions.find(identifier) != functions.end()) {
        auto& func = functions[identifier];

        std::map<std::string, std::string> origValues;
        for (size_t i = 0; i < func.first.size() && i < args.size(); ++i) {
            std::string argName = func.first[i];
            if (variables.find(argName) != variables.end()) {
                origValues[argName] = variables[argName];
            }
            variables[argName] = args[i]->evaluateString();
        }

        int result = func.second->evaluate();

        for (const auto& pair : origValues) {
            variables[pair.first] = pair.second;
        }

        return result;
    }
    std::cerr << "Error: Function '" << identifier << "' not defined." << std::endl;
    return 0;
}

void FuncCallNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "FunctionCall: " << identifier << "()" << std::endl;
    for (size_t i = 0; i < args.size(); ++i) {
        for (int j = 0; j < indent + 1; ++j) {
            std::cout << "  ";
        }
        std::cout << "Arg " << i << ": ";
        args[i]->print(indent + 2);
    }
}

bool FuncCallNode::validate(IContext* context) {
    for (size_t i = 0; i < args.size(); i++) {
        if (!args[i]->validate(context)) {
            errorMessage = "Error in argument " + std::to_string(i+1) + " of function '" +
                           identifier + "': " + args[i]->getErrorMessage();
            return false;
        }
    }

    if (!context->IsDefined(identifier, args.size())) {
        errorMessage = "Error: Function '" + identifier + "' with " +
                       std::to_string(args.size()) + " argument(s) is not defined";
        return false;
    }

    return true;
}

void FuncCallNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

FuncCallNode::~FuncCallNode() {
    for (auto arg : args) {
        delete arg;
    }
}
