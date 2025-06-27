#include "Context/Context.hpp"
#include "AST/Parameter.hpp"
#include "Types/Type.hpp"

Context::Context() : parent(nullptr) {
    // Register built-in functions
    functions["sin"] = {"x"};
    functions["cos"] = {"x"};
    functions["sqrt"] = {"x"};
    functions["rand"] = {};

    // Register built-in function signatures
    functionSignatures.emplace("sin", FunctionSignature({Type::getNumberType()}, Type::getNumberType()));
    functionSignatures.emplace("cos", FunctionSignature({Type::getNumberType()}, Type::getNumberType()));
    functionSignatures.emplace("sqrt", FunctionSignature({Type::getNumberType()}, Type::getNumberType()));
    functionSignatures.emplace("rand", FunctionSignature({}, Type::getNumberType()));
    
    // Register built-in constants
    variables.insert("PI");
    variableTypes["PI"] = Type::getNumberType();
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


bool Context::DefineVariable(const std::string& variable, Type* type) {
    // Si la variable ya existe en este contexto, no la redefinimos
    if (variables.find(variable) != variables.end()) {
        return false;
    }
    
    variables.insert(variable);
    variableTypes[variable] = type;
    return true;
}

Type* Context::GetVariableType(const std::string& variable) {
    // Buscar en el contexto actual
    auto it = variableTypes.find(variable);
    if (it != variableTypes.end()) {
        return it->second;
    }
    
    // Buscar en el contexto padre
    if (parent != nullptr) {
        Context* parentContext = dynamic_cast<Context*>(parent);
        if (parentContext) {
            return parentContext->GetVariableType(variable);
        }
    }
    
    // Si no se encuentra, retornar tipo desconocido
    return nullptr;
}
