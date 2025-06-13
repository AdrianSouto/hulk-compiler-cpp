#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <string>


class IContext;
class LLVMCodegenVisitor;

class ASTNode {
protected:
    std::string errorMessage;

public:
    virtual void print(int indent = 0) const = 0;
    virtual bool validate(IContext* context) = 0;
    virtual void accept(LLVMCodegenVisitor& visitor) = 0;
    virtual ~ASTNode() = default;

    const std::string& getErrorMessage() const;
    void setErrorMessage(const std::string& message);
};

#endif
