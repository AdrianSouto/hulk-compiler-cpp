#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include <string>
#include "Types/Type.hpp"
#include "AST/ExpressionNode.hpp"


class ASTNode;

class Attribute : public ASTNode {
public:
    std::string name;
    Type* type;
    ExpressionNode* initExpression;

    Attribute(const std::string& n, Type* t, ExpressionNode* expr)
        : name(n), type(t), initExpression(expr) {}

    bool hasType() const { return type != nullptr; }
    bool hasInitExpression() const { return initExpression != nullptr; }

    std::string toString() const {
        std::string result = name;
        if (hasType()) {
            result += ": " + type->toString();
        }
        if (hasInitExpression()) {
            result += " = <expression>";
        }
        return result;
    }


    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;

};

#endif