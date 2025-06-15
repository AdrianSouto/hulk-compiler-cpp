#include "Context/Context.hpp"
#include "AST/Parameter.hpp"
#include "Types/Type.hpp"

Context::Context() : parent(nullptr) {

    functions["sin"] = {"x"};
    functions["cos"] = {"x"};
    functions["sqrt"] = {"x"};
    functions["rand"] = {};


    functionSignatures.emplace("sin", FunctionSignature({Type::getNumberType()}, Type::getNumberType()));
    functionSignatures.emplace("cos", FunctionSignature({Type::getNumberType()}, Type::getNumberType()));
    functionSignatures.emplace("sqrt", FunctionSignature({Type::getNumberType()}, Type::getNumberType()));
    functionSignatures.emplace("rand", FunctionSignature({}, Type::getNumberType()));

    variables.insert("PI");
}

Context::Context(IContext* parent) : parent(parent) {}

bool Context::IsDefined(const std::string& variable) {
    return variables.find(variable) != variables.end() ||
           (parent != nullptr && parent->IsDefined(variable));
}

bool Context::IsDefined(const std::string& function, int args) {
    auto it = functions.find(function);
    if (it != functions.end() && it->second.size() == args) {
        return true;
    }
    return parent != nullptr && parent->IsDefined(function, args);
}

bool Context::Define(const std::string& variable) {

    if (variables.find(variable) != variables.end()) {
        return false;
    }
    variables.insert(variable);
    return true;
}

bool Context::Define(const std::string& function, const std::vector<std::string>& args) {
    auto it = functions.find(function);
    if (it != functions.end() && it->second.size() == args.size()) {
        return false;
    }
    functions[function] = args;
    return true;
}

bool Context::DefineFunction(const std::string& function, const std::vector<Parameter>& params, Type* returnType) {

    std::vector<std::string> paramNames;
    std::vector<Type*> paramTypes;
    
    for (const auto& param : params) {
        paramNames.push_back(param.name);

        Type* paramType = param.type ? param.type : Type::getNumberType();
        paramTypes.push_back(paramType);
    }
    

    Type* retType = returnType ? returnType : Type::getNumberType();
    

    auto it = functionSignatures.find(function);
    if (it != functionSignatures.end()) {
        functionSignatures.erase(it);
    }
    functionSignatures.emplace(function, FunctionSignature(paramTypes, retType));
    
    return true;
}

FunctionSignature* Context::GetFunctionSignature(const std::string& function, int args) {
    auto it = functionSignatures.find(function);
    if (it != functionSignatures.end() && it->second.parameterTypes.size() == args) {
        return &it->second;
    }
    

    if (parent != nullptr) {
        Context* parentContext = dynamic_cast<Context*>(parent);
        if (parentContext) {
            return parentContext->GetFunctionSignature(function, args);
        }
    }
    
    return nullptr;
}

IContext* Context::CreateChildContext() {
    return new Context(this);
}

bool Context::IsInMethod() {

    return false;
}

bool Context::HasParentMethod() {

    return false;
}
