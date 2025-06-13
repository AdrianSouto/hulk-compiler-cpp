#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "Context/IContext.hpp"
#include <unordered_set>
#include <unordered_map>

class Context : public IContext {
private:
    IContext* parent;
    std::unordered_set<std::string> variables;
    std::unordered_map<std::string, std::vector<std::string>> functions;

public:
    Context();
    explicit Context(IContext* parent);

    bool IsDefined(const std::string& variable) override;
    bool IsDefined(const std::string& function, int args) override;
    bool Define(const std::string& variable) override;
    bool Define(const std::string& function, const std::vector<std::string>& args) override;
    IContext* CreateChildContext() override;
    bool IsInMethod() override;
    bool HasParentMethod() override;
    ~Context() override = default;
};

#endif
