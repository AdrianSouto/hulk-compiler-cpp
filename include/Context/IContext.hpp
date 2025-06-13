#ifndef ICONTEXT_HPP
#define ICONTEXT_HPP

#include <string>
#include <vector>

class IContext {
public:
    virtual bool IsDefined(const std::string& variable) = 0;
    virtual bool IsDefined(const std::string& function, int args) = 0;
    virtual bool Define(const std::string& variable) = 0;
    virtual bool Define(const std::string& function, const std::vector<std::string>& args) = 0;
    virtual IContext* CreateChildContext() = 0;
    virtual bool IsInMethod() = 0;
    virtual bool HasParentMethod() = 0;
    virtual ~IContext() = default;
};

#endif
