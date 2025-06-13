#include "Expressions/MethodCallNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Globals.hpp"
#include <iostream>

MethodCallNode::~MethodCallNode() {
    delete object;
    for (auto arg : arguments) {
        delete arg;
    }
}

int MethodCallNode::evaluate() const {
    std::cout << "Calling method '" << methodName << "' on object" << std::endl;
    
    
    
    
    
    
    
    
    if (auto varNode = dynamic_cast<VariableNode*>(object)) {
        
        
        
        
        
        for (const auto& typePair : types) {
            TypeDefNode* typeDef = typePair.second;
            
            
            for (auto method : typeDef->methods) {
                if (auto funcDef = dynamic_cast<DefFuncNode*>(method)) {
                    if (funcDef->identifier == methodName) {
                        
                        std::cout << "Found method '" << methodName << "' in type '" << typePair.first << "'" << std::endl;
                        
                        
                        
                        if (funcDef->parameters.empty() && arguments.empty()) {
                            
                            if (auto methodVarNode = dynamic_cast<VariableNode*>(funcDef->expr)) {
                                
                                for (const auto& attr : typeDef->attributes) {
                                    if (attr.name == methodVarNode->identifier) {
                                        
                                        if (attr.initExpression) {
                                            std::cout << "Method '" << methodName << "' accesses attribute '" << attr.name << "'" << std::endl;
                                            return attr.initExpression->evaluate();
                                        }
                                    }
                                }
                            }
                            return funcDef->expr->evaluate();
                        }
                        
                        
                        
                        return funcDef->expr->evaluate();
                    }
                }
            }
        }
    }
    
    
    std::cout << "Could not resolve method '" << methodName << "'" << std::endl;
    return 0;
}

void MethodCallNode::execute() const {
    std::cout << "Executing method call: " << methodName << "()" << std::endl;
}

void MethodCallNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "MethodCall: ";
    object->print(0);
    std::cout << "." << methodName << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) std::cout << ", ";
        arguments[i]->print(0);
    }
    std::cout << ")" << std::endl;
}

bool MethodCallNode::validate(IContext* context) {
    
    if (!object->validate(context)) {
        errorMessage = "Error in method call object: " + object->getErrorMessage();
        return false;
    }
    
    
    for (size_t i = 0; i < arguments.size(); i++) {
        if (!arguments[i]->validate(context)) {
            errorMessage = "Error in argument " + std::to_string(i+1) + " of method '" +
                           methodName + "': " + arguments[i]->getErrorMessage();
            return false;
        }
    }
    
    
    
    
    return true;
}

void MethodCallNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}