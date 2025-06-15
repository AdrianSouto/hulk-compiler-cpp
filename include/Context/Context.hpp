#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Context/IContext.hpp"
#include <unordered_set>
#include <unordered_map>
#include <vector>

class Type;
class Parameter;

struct FunctionSignature {
    std::vector<Type*> parameterTypes;
    Type* returnType;
    
    FunctionSignature(const std::vector<Type*>& params, Type* ret) 
        : parameterTypes(params), returnType(ret) {}
};

class Context : public IContext {
private:
    IContext* parent;
    std::unordered_set<std::string> variables;
    std::unordered_map<std::string, std::vector<std::string>> functions;
    std::unordered_map<std::string, FunctionSignature> functionSignatures;

public:
    Context();
    explicit Context(IContext* parent);

    bool IsDefined(const std::string& variable) override;
    bool IsDefined(const std::string& function, int args) override;
    bool Define(const std::string& variable) override;
    bool Define(const std::string& function, const std::vector<std::string>& args) override;
    

    bool DefineFunction(const std::string& function, const std::vector<Parameter>& params, Type* returnType);
    FunctionSignature* GetFunctionSignature(const std::string& function, int args);
    
    IContext* CreateChildContext() override;
    bool IsInMethod() override;
    bool HasParentMethod() override;
    ~Context() override = default;
};

#endif
