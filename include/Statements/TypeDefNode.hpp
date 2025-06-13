#ifndef TYPE_DEF_NODE_HPP
#define TYPE_DEF_NODE_HPP

#include "AST/StatementNode.hpp"
#include "AST/Parameter.hpp"
#include "AST/Attribute.hpp"
#include "Types/Type.hpp"
#include <string>
#include <vector>

class TypeDefNode : public StatementNode {
public:
    std::string typeName;
    std::vector<Parameter> typeArguments;  
    std::vector<Attribute> attributes;     
    std::vector<StatementNode*> methods;   
    std::vector<ExpressionNode*> attributeExpressions; 
    
    
    std::string parentTypeName;            
    std::vector<ExpressionNode*> parentArgs; 
    
    TypeDefNode(const std::string& name, 
                const std::vector<Parameter>& args,
                const std::vector<Attribute>& attrs,
                const std::vector<StatementNode*>& meths = {});
    
    
    TypeDefNode(const std::string& name, 
                const std::vector<Parameter>& args,
                const std::vector<ASTNode*>& members);
    
    
    TypeDefNode(const std::string& name, 
                const std::vector<Parameter>& args,
                const std::string& parentName,
                const std::vector<ExpressionNode*>& parentArguments,
                const std::vector<ASTNode*>& members);

    void execute() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    
    
    bool validateTypeArguments(IContext* context);
    bool validateAttributes(IContext* context);
    bool validateAttributeInitialization(const Attribute& attr, IContext* context);
    bool validateTypeArgumentUsage(ExpressionNode* expr, IContext* context);
    Type* inferExpressionType(ExpressionNode* expr);
    Type* getTypeArgumentType(const std::string& argName);
    
    ~TypeDefNode();
};

#endif 