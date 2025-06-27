#ifndef TYPE_INSTANTIATION_NODE_HPP
#define TYPE_INSTANTIATION_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include <string>
#include <vector>

class TypeInstantiationNode : public ExpressionNode {
public:
    std::string typeName;
    std::vector<ExpressionNode*> arguments;
    
    TypeInstantiationNode(const std::string& name, const std::vector<ExpressionNode*>& args)
        : typeName(name), arguments(args) {}

    bool validate(IContext* context) override;
    Type* inferType(IContext* context) const override;
    void accept(LLVMCodegenVisitor& visitor) override;
    
    ~TypeInstantiationNode();
};

#endif