#include "Statements/DefFuncNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include <iostream>
#include <unordered_set>


DefFuncNode::DefFuncNode(const std::string& id, const std::vector<Parameter>& params, 
                         ExpressionNode* e, Type* retType)
    : identifier(id), parameters(params), returnType(retType), expr(e) {}


DefFuncNode::DefFuncNode(const std::string& id, const std::vector<std::string>& args, ExpressionNode* e)
    : identifier(id), returnType(nullptr), expr(e) {

    for (const auto& arg : args) {
        parameters.emplace_back(arg, nullptr);
    }
}

void DefFuncNode::execute() const {

    std::vector<std::string> argNames;
    for (const auto& param : parameters) {
        argNames.push_back(param.name);
    }
    functions[identifier] = std::make_pair(argNames, expr);
}

void DefFuncNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "DefFunc: " << identifier << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        std::cout << parameters[i].toString();
        if (i < parameters.size() - 1) std::cout << ", ";
    }
    std::cout << ")";
    
    if (returnType) {
        std::cout << ": " << returnType->toString();
    }
    std::cout << std::endl;

    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << "Body: ";
    expr->print(indent + 2);
}

bool DefFuncNode::validate(IContext* context) {
    IContext* innerContext = context->CreateChildContext();


    std::unordered_set<std::string> paramSet;
    std::vector<std::string> paramNames;
    
    for (const auto& param : parameters) {
        if (paramSet.find(param.name) != paramSet.end()) {
            errorMessage = "Error in function '" + identifier + "': Duplicate parameter name '" + param.name + "'";
            delete innerContext;
            return false;
        }
        paramSet.insert(param.name);
        paramNames.push_back(param.name);
        innerContext->Define(param.name);
    }


    if (!validateParameterTypes(innerContext)) {
        delete innerContext;
        return false;
    }


    if (!expr->validate(innerContext)) {
        errorMessage = "Error in function '" + identifier + "' body: " + expr->getErrorMessage();
        delete innerContext;
        return false;
    }


    if (!validateReturnType(innerContext)) {
        delete innerContext;
        return false;
    }


    if (!context->Define(identifier, paramNames)) {
        errorMessage = "Error: Function '" + identifier + "' with " +
                       std::to_string(parameters.size()) + " arguments is already defined";
        delete innerContext;
        return false;
    }

    delete innerContext;
    return true;
}

bool DefFuncNode::validateParameterTypes(IContext* context) {

    for (const auto& param : parameters) {
        if (!param.hasType()) {
            errorMessage = "Error in function '" + identifier + "': Parameter '" + param.name + "' must have an explicit type";
            return false;
        }
        
        if (param.type->getKind() == TypeKind::UNKNOWN) {
            errorMessage = "Error in function '" + identifier + "': Invalid type for parameter '" + param.name + "'";
            return false;
        }
    }
    return true;
}

bool DefFuncNode::validateReturnType(IContext* context) {

    if (!returnType) {
        errorMessage = "Error in function '" + identifier + "': Return type must be explicitly declared";
        return false;
    }
    

    if (returnType->getKind() == TypeKind::UNKNOWN) {
        errorMessage = "Error in function '" + identifier + "': Invalid return type";
        return false;
    }
    
    return true;
}

void DefFuncNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

DefFuncNode::~DefFuncNode() {
    delete expr;
}