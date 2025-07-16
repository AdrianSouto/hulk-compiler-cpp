#pragma once
#include <vector>
#include <memory>
#include "ParseTreeVisitor.hpp"
#include "ParseTree.hpp"

// Forward declarations
class ASTNode;
class Program;
class StatementNode;
class ExpressionNode;
class Parameter;
class VarDeclPair;
class Attribute;

namespace Parser {

class ASTBuilder : public ParseTreeVisitor {
private:
    // Helper methods for building AST nodes
    std::vector<StatementNode*> buildStatementList(ParseNode* node);
    StatementNode* buildStatement(ParseNode* node);
    ExpressionNode* buildExpression(ParseNode* node);
    ExpressionNode* buildPrimary(ParseNode* node);
    ExpressionNode* buildBinaryExpression(ParseNode* node);
    
    // Simple grammar builders
    ExpressionNode* buildTerm(ParseNode* node);
    ExpressionNode* buildFactor(ParseNode* node);
    ExpressionNode* buildAtom(ParseNode* node);
    ExpressionNode* buildExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildTermTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildFactorTail(ExpressionNode* left, ParseNode* node);
    
    // List builders
    std::vector<ExpressionNode*> buildExpressionList(ParseNode* node);
    std::vector<Parameter> buildParameterList(ParseNode* node);
    std::vector<VarDeclPair> buildVarDeclList(ParseNode* node);
    std::vector<Attribute> buildAttributeList(ParseNode* node);
    
    // Expression builders
    ExpressionNode* buildConditional(ParseNode* node);
    ExpressionNode* buildLetExpression(ParseNode* node);
    ExpressionNode* buildWhileLoop(ParseNode* node);
    ExpressionNode* buildForLoop(ParseNode* node);
    ExpressionNode* buildFunctionCall(ParseNode* node);
    ExpressionNode* buildMemberAccess(ParseNode* node);
    ExpressionNode* buildTypeInstantiation(ParseNode* node);
    ExpressionNode* buildBlock(ParseNode* node);
    
    // Binary expression builders
    ExpressionNode* buildOrExpression(ParseNode* node);
    ExpressionNode* buildOrExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildAndExpression(ParseNode* node);
    ExpressionNode* buildAndExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildEqualityExpression(ParseNode* node);
    ExpressionNode* buildEqualityExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildRelationalExpression(ParseNode* node);
    ExpressionNode* buildRelationalExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildAdditiveExpression(ParseNode* node);
    ExpressionNode* buildAdditiveExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildMultiplicativeExpression(ParseNode* node);
    ExpressionNode* buildMultiplicativeExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildPowerExpression(ParseNode* node);
    ExpressionNode* buildPowerExpressionTail(ExpressionNode* left, ParseNode* node);
    ExpressionNode* buildUnaryExpression(ParseNode* node);
    ExpressionNode* buildPostfixExpression(ParseNode* node);
    ExpressionNode* buildPostfixExpressionTail(ExpressionNode* left, ParseNode* node);
    
    // Helper builders
    std::vector<ExpressionNode*> buildArgumentList(ParseNode* node);
    Parameter buildParameter(ParseNode* node);
    VarDeclPair buildVarDecl(ParseNode* node);
    
    // Statement builders
    StatementNode* buildFunctionDef(ParseNode* node);
    StatementNode* buildTypeDef(ParseNode* node);
    StatementNode* buildPrintStatement(ParseNode* node);

public:
    ASTBuilder() = default;
    
    // Main visitor method
    ASTNode* visit(ParseNode* node) override;
    
    // Convert parse tree to AST
    Program* buildAST(ParseTree* parseTree);
};

} // namespace Parser