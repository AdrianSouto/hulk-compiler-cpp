#include "Statements/DefFuncNode.hpp"
#include "Context/IContext.hpp"
#include "Context/Context.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Expressions/ConcatenationNode.hpp"
#include <iostream>
#include <unordered_set>


DefFuncNode::DefFuncNode(const std::string& id, const std::vector<Parameter>& params, 
                         ExpressionNode* e, Type* retType)
    : identifier(id), parameters(params), returnType(retType), expr(e), isBlockBody(false) {}


DefFuncNode::DefFuncNode(const std::string& id, const std::vector<Parameter>& params,
                        const std::vector<StatementNode*>& stmts, Type* retType)
    : identifier(id), parameters(params), returnType(retType), expr(nullptr), statements(stmts), isBlockBody(true) {}


DefFuncNode::DefFuncNode(const std::string& id, const std::vector<std::string>& args, ExpressionNode* e)
    : identifier(id), returnType(nullptr), expr(e), isBlockBody(false) {

    for (const auto& arg : args) {
        parameters.emplace_back(arg, nullptr);
    }
}




bool DefFuncNode::validate(IContext* context) {
    // First, just register the function signature without validating the body
    std::unordered_set<std::string> paramSet;
    std::vector<std::string> paramNames;
    
    for (const auto& param : parameters) {
        if (paramSet.find(param.name) != paramSet.end()) {
            errorMessage = "Error in function '" + identifier + "': Duplicate parameter name '" + param.name + "'";
            return false;
        }
        paramSet.insert(param.name);
        paramNames.push_back(param.name);
    }

    if (!context->Define(identifier, paramNames)) {
        errorMessage = "Error: Function '" + identifier + "' with " +
                       std::to_string(parameters.size()) + " arguments is already defined";
        return false;
    }

    Context* ctx = dynamic_cast<Context*>(context);
    if (ctx) {
        Type* retType = returnType ? returnType : Type::getNumberType();
        ctx->DefineFunction(identifier, parameters, retType);
    }

    // The body validation will be done in a second pass
    return true;
}

bool DefFuncNode::validateBody(IContext* context) {
    IContext* innerContext = context->CreateChildContext();

    // Add parameters to inner context
    for (const auto& param : parameters) {
        innerContext->Define(param.name);
    }

    if (!validateParameterTypes(innerContext)) {
        delete innerContext;
        return false;
    }

    // Add the function itself to allow recursion
    std::vector<std::string> paramNames;
    for (const auto& param : parameters) {
        paramNames.push_back(param.name);
    }
    innerContext->Define(identifier, paramNames);

    if (isBlockBody) {
        for (auto stmt : statements) {
            if (!stmt->validate(innerContext)) {
                errorMessage = "Error in function '" + identifier + "' body: " + stmt->getErrorMessage();
                delete innerContext;
                return false;
            }
        }
    } else {
        if (!expr->validate(innerContext)) {
            errorMessage = "Error in function '" + identifier + "' body: " + expr->getErrorMessage();
            delete innerContext;
            return false;
        }
    }

    if (!validateReturnType(innerContext)) {
        delete innerContext;
        return false;
    }

    delete innerContext;
    return true;
}

bool DefFuncNode::validateParameterTypes(IContext* context) {
    bool isMethodInType = false;
    
    for (const auto& param : parameters) {
        if (param.name == "self") {
            isMethodInType = true;
            break;
        }
    }
    
    if (isMethodInType) {
        for (const auto& param : parameters) {
            if (!param.hasType() && param.name != "self") {
                errorMessage = "Error in method '" + identifier + "': Parameter '" + param.name + "' must have an explicit type";
                return false;
            }
            
            if (param.hasType() && param.type->getKind() == TypeKind::UNKNOWN) {
                errorMessage = "Error in method '" + identifier + "': Invalid type for parameter '" + param.name + "'";
                return false;
            }
        }
    }

    return true;
}

bool DefFuncNode::validateReturnType(IContext* context) {
    bool isMethodInType = false;
    for (const auto& param : parameters) {
        if (param.name == "self") {
            isMethodInType = true;
            break;
        }
    }
    
    if (isMethodInType) {
        if (!returnType) {
            errorMessage = "Error in method '" + identifier + "': Return type must be explicitly declared";
            return false;
        }
        
        if (returnType->getKind() == TypeKind::UNKNOWN) {
            errorMessage = "Error in method '" + identifier + "': Invalid return type";
            return false;
        }
    }

    return true;
}

Type* DefFuncNode::inferReturnType(IContext* context) {
    if (isBlockBody) {

        return Type::getNumberType();
    }

    if (dynamic_cast<NumberNode*>(expr)) {
        return Type::getNumberType();
    }
    
    if (dynamic_cast<StringLiteralNode*>(expr)) {
        return Type::getStringType();
    }
    
    if (dynamic_cast<BooleanNode*>(expr)) {
        return Type::getBooleanType();
    }
    
    if (dynamic_cast<ConcatenationNode*>(expr)) {
        return Type::getStringType();
    }
    
    return Type::getNumberType();
}

void DefFuncNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

DefFuncNode::~DefFuncNode() {
    if (!isBlockBody) {
        delete expr;
    } else {
        for (auto stmt : statements) {
            delete stmt;
        }
    }
}

