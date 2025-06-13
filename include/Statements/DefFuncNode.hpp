#ifndef DEF_FUNC_NODE_HPP
#define DEF_FUNC_NODE_HPP

#include "AST/StatementNode.hpp"
#include "AST/ExpressionNode.hpp"
#include "AST/Parameter.hpp"
#include "Types/Type.hpp"
#include <string>
#include <vector>

class DefFuncNode : public StatementNode {
public:
    std::string identifier;
    std::vector<Parameter> parameters;  
    Type* returnType;                   
    ExpressionNode* expr;


    DefFuncNode(const std::string& id, const std::vector<Parameter>& params, 
                ExpressionNode* expr, Type* retType = nullptr);
    

    DefFuncNode(const std::string& id, const std::vector<std::string>& args, ExpressionNode* expr);

    void execute() const override;
    void print(int indent = 0) const override;
    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    

    bool validateParameterTypes(IContext* context);
    bool validateReturnType(IContext* context);
    
    ~DefFuncNode();
};

#endif