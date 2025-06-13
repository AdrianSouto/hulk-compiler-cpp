#include "Context/Context.hpp"

Context::Context() : parent(nullptr) {

    functions["sin"] = {"x"};
    functions["cos"] = {"x"};
    functions["sqrt"] = {"x"};


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

IContext* Context::CreateChildContext() {
    return new Context(this);
}

bool Context::IsInMethod() {

    return false;
}

bool Context::HasParentMethod() {

    return false;
}
