#ifndef FOR_RANGE_NODE_HPP
#define FOR_RANGE_NODE_HPP

#include "AST/ExpressionNode.hpp"
#include "AST/ASTNode.hpp"
#include <string>

class ForRangeNode : public ExpressionNode {
public:
    std::string loopVar;
    ExpressionNode* startExpr;
    ExpressionNode* endExpr;
    ASTNode* body;

    ForRangeNode(const std::string& var, ExpressionNode* start, ExpressionNode* end, ASTNode* b);

    bool validate(IContext* context) override;
    void accept(LLVMCodegenVisitor& visitor) override;
    ~ForRangeNode();
};

#endif