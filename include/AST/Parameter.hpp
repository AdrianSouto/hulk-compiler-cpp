#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include <string>
#include "Types/Type.hpp"

class Parameter {
public:
    std::string name;
    Type* type;
    
    Parameter(const std::string& n, Type* t = nullptr) : name(n), type(t) {}
    
    bool hasType() const { return type != nullptr; }
    
    std::string toString() const {
        if (hasType()) {
            return name + ": " + type->toString();
        }
        return name;
    }
};

#endif