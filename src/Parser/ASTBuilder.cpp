#include "Parser/ASTBuilder.hpp"
#include "Parser/ParseTree.hpp"
#include "AST/Program.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Statements/PrintStatementNode.hpp"
#include "Statements/ExpressionStatementNode.hpp"
#include "Statements/BlockNode.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Expressions/AdditionNode.hpp"
#include "Expressions/SubtractionNode.hpp"
#include "Expressions/MultiplicationNode.hpp"
#include "Expressions/DivisionNode.hpp"
#include "Expressions/ModuloNode.hpp"
#include "Expressions/PowerNode.hpp"
#include "Expressions/ConcatenationNode.hpp"
#include "Expressions/EqualNode.hpp"
#include "Expressions/NotEqualNode.hpp"
#include "Expressions/LessNode.hpp"
#include "Expressions/LessEqNode.hpp"
#include "Expressions/GreaterNode.hpp"
#include "Expressions/GreaterEqNode.hpp"
#include "Expressions/AndNode.hpp"
#include "Expressions/OrNode.hpp"
#include "Expressions/NotNode.hpp"
#include "Expressions/UnaryMinusNode.hpp"
#include "Expressions/ConditionalNode.hpp"
#include "Expressions/WhileNode.hpp"
#include "Expressions/ForRangeNode.hpp"
#include "Expressions/LetExprNode.hpp"
#include "Expressions/FuncCallNode.hpp"
#include "Expressions/MethodCallNode.hpp"
#include "Expressions/MemberAccessNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/AssignmentNode.hpp"
#include "Expressions/BlockExprNode.hpp"
#include "Expressions/BaseCallNode.hpp"
#include "Expressions/PrintExpressionNode.hpp"
#include "Statements/LetVarNode.hpp"
#include <stdexcept>
#include <iostream>

namespace Parser {

void printParseTree(ParseNode* node, int depth = 0) {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::cerr << indent << "Node: " << node->symbol.value;
    if (node->symbol.isTerminal()) {
        std::cerr << " (Terminal: '" << node->token.lexeme << "')";
    }
    std::cerr << " [" << node->children.size() << " children]" << std::endl;
    
    for (const auto& child : node->children) {
        printParseTree(child.get(), depth + 1);
    }
}

Program* ASTBuilder::buildAST(ParseTree* parseTree) {
    if (!parseTree || !parseTree->root) {
        return nullptr;
    }
    
    // Check if root is Program
    if (parseTree->root->symbol.value == "Program") {
        Program* program = new Program();
        
        // New grammar: Program -> StatementList
        if (!parseTree->root->children.empty()) {
            std::vector<StatementNode*> statements = buildStatementList(parseTree->root->children[0].get());
            program->Statements = statements;
        }
        
        return program;
    }
    
    throw std::runtime_error("Failed to build AST: root is not a Program node");
}

ASTNode* ASTBuilder::visit(ParseNode* node) {
    if (!node) return nullptr;
    
    const std::string& symbol = node->symbol.value;
    
    // Program
    if (symbol == "Program") {
        // Program is not an ASTNode, handle it separately
        return nullptr;
    }
    
    // Statements
    if (symbol == "Statement") {
        return buildStatement(node);
    }
    
    if (symbol == "FunctionDef") {
        return buildFunctionDef(node);
    }
    
    if (symbol == "TypeDef") {
        return buildTypeDef(node);
    }
    
    if (symbol == "PrintStatement") {
        return buildPrintStatement(node);
    }
    
    // Expressions
    if (symbol == "Expression") {
        return buildExpression(node);
    }
    
    if (symbol == "Primary") {
        return buildPrimary(node);
    }
    
    // Handle other specific nodes
    if (symbol == "ConditionalExpr") {
        return buildConditional(node);
    }
    
    if (symbol == "WhileExpr") {
        return buildWhileLoop(node);
    }
    
    if (symbol == "ForExpr") {
        return buildForLoop(node);
    }
    
    if (symbol == "LetExpr") {
        return buildLetExpression(node);
    }
    
    if (symbol == "TypeInstantiation") {
        return buildTypeInstantiation(node);
    }
    
    if (symbol == "Block") {
        return buildBlock(node);
    }
    
    // If we reach here, try to process children
    if (!node->children.empty()) {
        return visit(node->children[0].get());
    }
    
    return nullptr;
}

std::vector<StatementNode*> ASTBuilder::buildStatementList(ParseNode* node) {
    std::vector<StatementNode*> statements;
    
    if (!node || node->symbol.value != "StatementList") {
        return statements;
    }
    
    // StatementList -> Statement StatementList | ε
    if (!node->children.empty()) {
        StatementNode* stmt = dynamic_cast<StatementNode*>(visit(node->children[0].get()));
        if (stmt) {
            statements.push_back(stmt);
        }
        
        if (node->children.size() > 1) {
            auto moreStmts = buildStatementList(node->children[1].get());
            statements.insert(statements.end(), moreStmts.begin(), moreStmts.end());
        }
    }
    
    return statements;
}

StatementNode* ASTBuilder::buildStatement(ParseNode* node) {
    if (!node || node->children.empty()) return nullptr;
    
    // New grammar: Statement -> Expression SEMICOLON
    if (node->children.size() >= 2 && node->children[1]->symbol.value == "SEMICOLON") {
        ExpressionNode* expr = buildExpression(node->children[0].get());
        return new ExpressionStatementNode(expr);
    }
    
    // Check first child to determine statement type (for other statement types)
    ParseNode* firstChild = node->children[0].get();
    
    if (firstChild->symbol.value == "FunctionDef") {
        return buildFunctionDef(firstChild);
    }
    else if (firstChild->symbol.value == "TypeDef") {
        return buildTypeDef(firstChild);
    }
    else if (firstChild->symbol.value == "PrintStatement") {
        return buildPrintStatement(firstChild);
    }
    else if (firstChild->symbol.value == "Expression") {
        ExpressionNode* expr = buildExpression(firstChild);
        return new ExpressionStatementNode(expr);
    }
    
    return nullptr;
}

ExpressionNode* ASTBuilder::buildExpression(ParseNode* node) {
    if (!node || node->children.empty()) return nullptr;
    
    // For simple grammar: Expression -> Term ExpressionTail
    if (node->children.size() >= 2) {
        ExpressionNode* left = buildTerm(node->children[0].get());
        if (left) {
            return buildExpressionTail(left, node->children[1].get());
        }
    }
    
    return nullptr;
}

ExpressionNode* ASTBuilder::buildTerm(ParseNode* node) {
    if (!node || node->children.empty()) return nullptr;
    
    // Term -> Factor TermTail
    if (node->children.size() >= 2) {
        ExpressionNode* left = buildFactor(node->children[0].get());
        if (left) {
            return buildTermTail(left, node->children[1].get());
        }
    }
    
    return nullptr;
}

ExpressionNode* ASTBuilder::buildFactor(ParseNode* node) {
    if (!node || node->children.empty()) return nullptr;
    
    // Factor -> Atom FactorTail
    if (node->children.size() >= 2) {
        ExpressionNode* left = buildAtom(node->children[0].get());
        return buildFactorTail(left, node->children[1].get());
    }
    
    return nullptr;
}

ExpressionNode* ASTBuilder::buildAtom(ParseNode* node) {
    if (!node || node->children.empty()) return nullptr;
    
    ParseNode* firstChild = node->children[0].get();
    std::string symbol = firstChild->symbol.value;
    
    if (symbol == "NUMBER") {
        return new NumberNode(std::stod(firstChild->token.lexeme));
    }
    else if (symbol == "STRING") {
        // Remove quotes from string literal
        std::string str = firstChild->token.lexeme;
        if (str.length() >= 2 && str[0] == '"' && str[str.length()-1] == '"') {
            str = str.substr(1, str.length() - 2);
        }
        return new StringLiteralNode(str);
    }
    else if (symbol == "TRUE") {
        return new BooleanNode(true);
    }
    else if (symbol == "FALSE") {
        return new BooleanNode(false);
    }
    else if (symbol == "IDENTIFIER") {
        return new VariableNode(firstChild->token.lexeme);
    }
    else if (symbol == "LPAREN" && node->children.size() >= 3) {
        // Parenthesized expression: LPAREN Expression RPAREN
        return buildExpression(node->children[1].get());
    }
    else if (symbol == "PRINT" && node->children.size() >= 4) {
        // Print expression: PRINT LPAREN Expression RPAREN
        ExpressionNode* expr = buildExpression(node->children[2].get());
        return new PrintExpressionNode(expr);
    }
    
    return nullptr;
}

ExpressionNode* ASTBuilder::buildExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // ExpressionTail -> PLUS Term ExpressionTail | MINUS Term ExpressionTail | ε
    if (node->children.size() >= 3) {
        std::string op = node->children[0]->symbol.value;
        if (op == "PLUS" || op == "MINUS") {
            ExpressionNode* right = buildTerm(node->children[1].get());
            ExpressionNode* result = nullptr;
            
            if (op == "PLUS") {
                result = new AdditionNode(left, right);
            } else if (op == "MINUS") {
                result = new SubtractionNode(left, right);
            }
            
            return buildExpressionTail(result, node->children[2].get());
        }
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildTermTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // TermTail -> MULTIPLY Factor TermTail | DIVIDE Factor TermTail | ε
    if (node->children.size() >= 3) {
        std::string op = node->children[0]->symbol.value;
        if (op == "MULTIPLY" || op == "DIVIDE") {
            ExpressionNode* right = buildFactor(node->children[1].get());
            ExpressionNode* result = nullptr;
            
            if (op == "MULTIPLY") {
                result = new MultiplicationNode(left, right);
            } else if (op == "DIVIDE") {
                result = new DivisionNode(left, right);
            }
            
            return buildTermTail(result, node->children[2].get());
        }
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildFactorTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // FactorTail -> POWER Atom FactorTail | ε
    if (node->children.size() >= 3 && node->children[0]->symbol.value == "POWER") {
        ExpressionNode* right = buildAtom(node->children[1].get());
        ExpressionNode* powerNode = new PowerNode(left, right);
        return buildFactorTail(powerNode, node->children[2].get());
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildOrExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // OrExpr -> AndExpr OrExprTail
    ExpressionNode* left = buildAndExpression(node->children[0].get());
    return buildOrExpressionTail(left, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildOrExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // OrExprTail -> OR AndExpr OrExprTail | ε
    if (node->children.size() >= 3 && node->children[0]->symbol.value == "OR") {
        ExpressionNode* right = buildAndExpression(node->children[1].get());
        ExpressionNode* orNode = new OrNode(left, right);
        return buildOrExpressionTail(orNode, node->children[2].get());
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildAndExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // AndExpr -> EqualityExpr AndExprTail
    ExpressionNode* left = buildEqualityExpression(node->children[0].get());
    return buildAndExpressionTail(left, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildAndExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // AndExprTail -> AND EqualityExpr AndExprTail | ε
    if (node->children.size() >= 3 && node->children[0]->symbol.value == "AND") {
        ExpressionNode* right = buildEqualityExpression(node->children[1].get());
        ExpressionNode* andNode = new AndNode(left, right);
        return buildAndExpressionTail(andNode, node->children[2].get());
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildEqualityExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // EqualityExpr -> RelationalExpr EqualityExprTail
    ExpressionNode* left = buildRelationalExpression(node->children[0].get());
    return buildEqualityExpressionTail(left, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildEqualityExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // EqualityExprTail -> EQEQ RelationalExpr EqualityExprTail | NOTEQ RelationalExpr EqualityExprTail | ε
    if (node->children.size() >= 3) {
        std::string op = node->children[0]->symbol.value;
        if (op == "EQEQ" || op == "NOTEQ") {
            ExpressionNode* right = buildRelationalExpression(node->children[1].get());
            ExpressionNode* result = nullptr;
            
            if (op == "EQEQ") {
                result = new EqualNode(left, right);
            } else {
                result = new NotEqualNode(left, right);
            }
            
            return buildEqualityExpressionTail(result, node->children[2].get());
        }
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildRelationalExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // RelationalExpr -> AdditiveExpr RelationalExprTail
    ExpressionNode* left = buildAdditiveExpression(node->children[0].get());
    return buildRelationalExpressionTail(left, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildRelationalExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // RelationalExprTail -> LESS AdditiveExpr RelationalExprTail | ... | ε
    if (node->children.size() >= 3) {
        std::string op = node->children[0]->symbol.value;
        if (op == "LESS" || op == "LESSEQ" || op == "GREATER" || op == "GREATEREQ") {
            ExpressionNode* right = buildAdditiveExpression(node->children[1].get());
            ExpressionNode* result = nullptr;
            
            if (op == "LESS") {
                result = new LessNode(left, right);
            } else if (op == "LESSEQ") {
                result = new LessEqNode(left, right);
            } else if (op == "GREATER") {
                result = new GreaterNode(left, right);
            } else if (op == "GREATEREQ") {
                result = new GreaterEqNode(left, right);
            }
            
            return buildRelationalExpressionTail(result, node->children[2].get());
        }
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildAdditiveExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // AdditiveExpr -> MultiplicativeExpr AdditiveExprTail
    ExpressionNode* left = buildMultiplicativeExpression(node->children[0].get());
    return buildAdditiveExpressionTail(left, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildAdditiveExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // AdditiveExprTail -> PLUS MultiplicativeExpr AdditiveExprTail | ... | ε
    if (node->children.size() >= 3) {
        std::string op = node->children[0]->symbol.value;
        if (op == "PLUS" || op == "MINUS" || op == "CONCAT" || op == "CONCAT_SPACE") {
            ExpressionNode* right = buildMultiplicativeExpression(node->children[1].get());
            ExpressionNode* result = nullptr;
            
            if (op == "PLUS") {
                result = new AdditionNode(left, right);
            } else if (op == "MINUS") {
                result = new SubtractionNode(left, right);
            } else if (op == "CONCAT" || op == "CONCAT_SPACE") {
                // TODO: Handle CONCAT_SPACE differently if needed
                result = new ConcatenationNode(left, right);
            }
            
            return buildAdditiveExpressionTail(result, node->children[2].get());
        }
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildMultiplicativeExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // MultiplicativeExpr -> PowerExpr MultiplicativeExprTail
    ExpressionNode* left = buildPowerExpression(node->children[0].get());
    return buildMultiplicativeExpressionTail(left, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildMultiplicativeExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // MultiplicativeExprTail -> MULTIPLY PowerExpr MultiplicativeExprTail | ... | ε
    if (node->children.size() >= 3) {
        std::string op = node->children[0]->symbol.value;
        if (op == "MULTIPLY" || op == "DIVIDE" || op == "MODULO") {
            ExpressionNode* right = buildPowerExpression(node->children[1].get());
            ExpressionNode* result = nullptr;
            
            if (op == "MULTIPLY") {
                result = new MultiplicationNode(left, right);
            } else if (op == "DIVIDE") {
                result = new DivisionNode(left, right);
            } else if (op == "MODULO") {
                result = new ModuloNode(left, right);
            }
            
            return buildMultiplicativeExpressionTail(result, node->children[2].get());
        }
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildPowerExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // PowerExpr -> UnaryExpr PowerExprTail
    ExpressionNode* left = buildUnaryExpression(node->children[0].get());
    return buildPowerExpressionTail(left, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildPowerExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // PowerExprTail -> POWER UnaryExpr PowerExprTail | ε
    if (node->children.size() >= 3 && node->children[0]->symbol.value == "POWER") {
        ExpressionNode* right = buildUnaryExpression(node->children[1].get());
        ExpressionNode* powerNode = new PowerNode(left, right);
        return buildPowerExpressionTail(powerNode, node->children[2].get());
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildUnaryExpression(ParseNode* node) {
    if (!node || node->children.empty()) return nullptr;
    
    // UnaryExpr -> NOT UnaryExpr | MINUS UnaryExpr | PostfixExpr
    if (node->children.size() >= 2) {
        std::string op = node->children[0]->symbol.value;
        if (op == "NOT") {
            ExpressionNode* operand = buildUnaryExpression(node->children[1].get());
            return new NotNode(operand);
        } else if (op == "MINUS") {
            ExpressionNode* operand = buildUnaryExpression(node->children[1].get());
            return new UnaryMinusNode(operand);
        }
    }
    
    // PostfixExpr
    return buildPostfixExpression(node->children[0].get());
}

ExpressionNode* ASTBuilder::buildPostfixExpression(ParseNode* node) {
    if (!node || node->children.size() < 2) return nullptr;
    
    // PostfixExpr -> Primary PostfixExprTail
    ExpressionNode* primary = buildPrimary(node->children[0].get());
    return buildPostfixExpressionTail(primary, node->children[1].get());
}

ExpressionNode* ASTBuilder::buildPostfixExpressionTail(ExpressionNode* left, ParseNode* node) {
    if (!node || node->children.empty()) return left;
    
    // PostfixExprTail -> DOT IDENTIFIER PostfixExprTail | LPAREN ArgumentList RPAREN PostfixExprTail | ASSIGN Expression | ε
    if (node->children.size() >= 2) {
        std::string first = node->children[0]->symbol.value;
        
        if (first == "DOT" && node->children.size() >= 3) {
            // Member access
            std::string memberName = node->children[1]->token.lexeme;
            ExpressionNode* memberAccess = new MemberAccessNode(left, memberName);
            return buildPostfixExpressionTail(memberAccess, node->children[2].get());
        }
        else if (first == "LPAREN" && node->children.size() >= 4) {
            // Function call
            std::vector<ExpressionNode*> args = buildArgumentList(node->children[1].get());
            ExpressionNode* funcCall = nullptr;
            
            if (dynamic_cast<VariableNode*>(left)) {
                // Regular function call
                funcCall = new FuncCallNode(dynamic_cast<VariableNode*>(left)->identifier, args);
            } else if (dynamic_cast<MemberAccessNode*>(left)) {
                // Method call
                MemberAccessNode* memberAccess = dynamic_cast<MemberAccessNode*>(left);
                funcCall = new MethodCallNode(memberAccess->object, memberAccess->memberName, args);
            } else {
                // Expression call (not typical in HULK but handle it)
                funcCall = new FuncCallNode("", args);
            }
            
            return buildPostfixExpressionTail(funcCall, node->children[3].get());
        }
        else if (first == "ASSIGN" && node->children.size() >= 2) {
            // Assignment
            ExpressionNode* value = buildExpression(node->children[1].get());
            
            // Check if left is a variable
            if (VariableNode* var = dynamic_cast<VariableNode*>(left)) {
                return new AssignmentNode(var->identifier, value);
            } else if (MemberAccessNode* memberAccess = dynamic_cast<MemberAccessNode*>(left)) {
                // Handle member assignment differently
                // TODO: Create MemberAssignmentNode if needed
                return new AssignmentNode(memberAccess->memberName, value);
            } else {
                // Invalid assignment target
                throw std::runtime_error("Invalid assignment target");
            }
        }
    }
    
    return left;
}

ExpressionNode* ASTBuilder::buildPrimary(ParseNode* node) {
    if (!node || node->children.empty()) return nullptr;
    
    ParseNode* firstChild = node->children[0].get();
    std::string symbol = firstChild->symbol.value;
    
    if (symbol == "NUMBER") {
        return new NumberNode(std::stod(firstChild->token.lexeme));
    }
    else if (symbol == "STRING") {
        // Remove quotes from string literal
        std::string str = firstChild->token.lexeme;
        if (str.length() >= 2 && str[0] == '"' && str[str.length()-1] == '"') {
            str = str.substr(1, str.length() - 2);
        }
        return new StringLiteralNode(str);
    }
    else if (symbol == "TRUE") {
        return new BooleanNode(true);
    }
    else if (symbol == "FALSE") {
        return new BooleanNode(false);
    }
    else if (symbol == "IDENTIFIER") {
        return new VariableNode(firstChild->token.lexeme);
    }
    else if (symbol == "LPAREN" && node->children.size() >= 3) {
        // Parenthesized expression
        return buildExpression(node->children[1].get());
    }
    else if (symbol == "ConditionalExpr") {
        return buildConditional(firstChild);
    }
    else if (symbol == "WhileExpr") {
        return buildWhileLoop(firstChild);
    }
    else if (symbol == "ForExpr") {
        return buildForLoop(firstChild);
    }
    else if (symbol == "TypeInstantiation") {
        return buildTypeInstantiation(firstChild);
    }
    else if (symbol == "Block") {
        return buildBlock(firstChild);
    }
    else if (symbol == "BASE" && node->children.size() >= 4) {
        // Base call
        std::vector<ExpressionNode*> args = buildArgumentList(node->children[2].get());
        return new BaseCallNode(args);
    }
    
    return nullptr;
}

ExpressionNode* ASTBuilder::buildConditional(ParseNode* node) {
    if (!node || node->children.size() < 8) return nullptr;
    
    // ConditionalExpr -> IF LPAREN Expression RPAREN Expression ElifList ELSE Expression
    ExpressionNode* condition = buildExpression(node->children[2].get());
    ExpressionNode* thenExpr = buildExpression(node->children[4].get());
    ExpressionNode* elseExpr = buildExpression(node->children[7].get());
    
    // Handle elif list
    std::vector<std::pair<ExpressionNode*, ExpressionNode*>> elifPairs;
    ParseNode* elifList = node->children[5].get();
    
    while (elifList && !elifList->children.empty()) {
        // ElifList -> ELIF LPAREN Expression RPAREN Expression ElifList
        if (elifList->children.size() >= 6) {
            ExpressionNode* elifCond = buildExpression(elifList->children[2].get());
            ExpressionNode* elifThen = buildExpression(elifList->children[4].get());
            elifPairs.push_back({elifCond, elifThen});
            elifList = elifList->children[5].get();
        } else {
            break;
        }
    }
    
    // Build conditional with branches
    ConditionalNode* conditional = new ConditionalNode();
    
    // Add main if branch
    conditional->addBranch(condition, thenExpr);
    
    // Add elif branches
    for (const auto& elifPair : elifPairs) {
        conditional->addBranch(elifPair.first, elifPair.second);
    }
    
    // Set else branch
    conditional->setElse(elseExpr);
    
    return conditional;
}

ExpressionNode* ASTBuilder::buildWhileLoop(ParseNode* node) {
    if (!node || node->children.size() < 5) return nullptr;
    
    // WhileExpr -> WHILE LPAREN Expression RPAREN Expression
    ExpressionNode* condition = buildExpression(node->children[2].get());
    ExpressionNode* body = buildExpression(node->children[4].get());
    
    return new WhileNode(condition, body);
}

ExpressionNode* ASTBuilder::buildForLoop(ParseNode* node) {
    if (!node || node->children.size() < 7) return nullptr;
    
    // ForExpr -> FOR LPAREN IDENTIFIER IN Expression RPAREN Expression
    std::string varName = node->children[2]->token.lexeme;
    ExpressionNode* iterable = buildExpression(node->children[4].get());
    ExpressionNode* body = buildExpression(node->children[6].get());
    
    // HULK uses range-based for loops
    // TODO: Parse range expression properly (e.g., "1..10")
    // For now, assume iterable is the end expression and start is 0
    ExpressionNode* startExpr = new NumberNode(0);
    return new ForRangeNode(varName, startExpr, iterable, body);
}

ExpressionNode* ASTBuilder::buildLetExpression(ParseNode* node) {
    if (!node || node->children.size() < 4) return nullptr;
    
    // LetExpr -> LET VarDeclList IN Expression
    std::vector<VarDeclPair> declarations = buildVarDeclList(node->children[1].get());
    ExpressionNode* body = buildExpression(node->children[3].get());
    
    return new LetExprNode(declarations, body);
}

ExpressionNode* ASTBuilder::buildTypeInstantiation(ParseNode* node) {
    if (!node || node->children.size() < 5) return nullptr;
    
    // TypeInstantiation -> NEW IDENTIFIER LPAREN ArgumentList RPAREN
    std::string typeName = node->children[1]->token.lexeme;
    std::vector<ExpressionNode*> args = buildArgumentList(node->children[3].get());
    
    return new TypeInstantiationNode(typeName, args);
}

ExpressionNode* ASTBuilder::buildBlock(ParseNode* node) {
    if (!node || node->children.size() < 3) return nullptr;
    
    // Block -> LBRACE ExpressionList RBRACE
    std::vector<ExpressionNode*> expressions = buildExpressionList(node->children[1].get());
    
    return new BlockExprNode(expressions);
}

StatementNode* ASTBuilder::buildFunctionDef(ParseNode* node) {
    if (!node || node->children.size() < 6) return nullptr;
    
    // FunctionDef -> FUNCTION IDENTIFIER LPAREN ParameterList RPAREN TypeAnnotationOpt Block/Expression
    std::string name = node->children[1]->token.lexeme;
    std::vector<Parameter> params = buildParameterList(node->children[3].get());
    
    // Get return type if specified
    std::string returnType = "";
    ParseNode* typeAnnotOpt = node->children[5].get();
    if (typeAnnotOpt && !typeAnnotOpt->children.empty()) {
        // TypeAnnotationOpt -> TypeAnnotation
        ParseNode* typeAnnot = typeAnnotOpt->children[0].get();
        if (typeAnnot && typeAnnot->children.size() >= 2) {
            // TypeAnnotation -> COLON IDENTIFIER
            returnType = typeAnnot->children[1]->token.lexeme;
        }
    }
    
    // Get body (either block or expression)
    ExpressionNode* body = nullptr;
    if (node->children.size() >= 7) {
        if (node->children[6]->symbol.value == "Block") {
            body = buildBlock(node->children[6].get());
        } else if (node->children[6]->symbol.value == "ARROW" && node->children.size() >= 8) {
            // Arrow function
            body = buildExpression(node->children[7].get());
        }
    }
    
    // Convert return type string to Type*
    Type* retType = nullptr;
    if (!returnType.empty()) {
        // TODO: Implement proper type resolution
        // For now, check for built-in types
        if (returnType == "Number") {
            retType = Type::getNumberType();
        } else if (returnType == "String") {
            retType = Type::getStringType();
        } else if (returnType == "Boolean") {
            retType = Type::getBooleanType();
        } else if (returnType == "Object") {
            retType = Type::getObjectType();
        }
        // For user-defined types, leave as nullptr for now
    }
    
    return new DefFuncNode(name, params, body, retType);
}

StatementNode* ASTBuilder::buildTypeDef(ParseNode* node) {
    if (!node || node->children.size() < 7) return nullptr;
    
    // TypeDef -> TYPE IDENTIFIER InheritsClause LBRACE MemberList RBRACE SEMICOLON
    std::string name = node->children[1]->token.lexeme;
    
    // Get parent type if inherits
    std::string parentType = "";
    ParseNode* inheritsClause = node->children[2].get();
    if (inheritsClause && inheritsClause->children.size() >= 2) {
        // InheritsClause -> INHERITS IDENTIFIER
        parentType = inheritsClause->children[1]->token.lexeme;
    }
    
    // Get members
    std::vector<Attribute> attributes;
    std::vector<DefFuncNode*> methods;
    
    ParseNode* memberList = node->children[4].get();
    while (memberList && !memberList->children.empty()) {
        // MemberList -> Member MemberList
        ParseNode* member = memberList->children[0].get();
        
        if (member && member->children.size() >= 3) {
            std::string memberName = member->children[0]->token.lexeme;
            
            // Check if it's an attribute or method
            if (member->children[2]->symbol.value == "EQUALS") {
                // Attribute: IDENTIFIER TypeAnnotationOpt EQUALS Expression SEMICOLON
                std::string attrType = "";
                ParseNode* typeAnnotOpt = member->children[1].get();
                if (typeAnnotOpt && !typeAnnotOpt->children.empty()) {
                    ParseNode* typeAnnot = typeAnnotOpt->children[0].get();
                    if (typeAnnot && typeAnnot->children.size() >= 2) {
                        attrType = typeAnnot->children[1]->token.lexeme;
                    }
                }
                
                ExpressionNode* initValue = buildExpression(member->children[3].get());
                Type* attrTypePtr = nullptr;
                if (!attrType.empty()) {
                    // TODO: Implement proper type resolution
                    // For now, check for built-in types
                    if (attrType == "Number") {
                        attrTypePtr = Type::getNumberType();
                    } else if (attrType == "String") {
                        attrTypePtr = Type::getStringType();
                    } else if (attrType == "Boolean") {
                        attrTypePtr = Type::getBooleanType();
                    } else if (attrType == "Object") {
                        attrTypePtr = Type::getObjectType();
                    }
                    // For user-defined types, leave as nullptr for now
                }
                attributes.push_back(Attribute(memberName, attrTypePtr, initValue));
            }
            else if (member->children[1]->symbol.value == "LPAREN") {
                // Method: IDENTIFIER LPAREN ParameterList RPAREN TypeAnnotationOpt Block/Expression
                std::vector<Parameter> params = buildParameterList(member->children[2].get());
                
                std::string returnType = "";
                ParseNode* typeAnnotOpt = member->children[4].get();
                if (typeAnnotOpt && !typeAnnotOpt->children.empty()) {
                    ParseNode* typeAnnot = typeAnnotOpt->children[0].get();
                    if (typeAnnot && typeAnnot->children.size() >= 2) {
                        returnType = typeAnnot->children[1]->token.lexeme;
                    }
                }
                
                ExpressionNode* body = nullptr;
                if (member->children.size() >= 6) {
                    if (member->children[5]->symbol.value == "Block") {
                        body = buildBlock(member->children[5].get());
                    } else if (member->children[5]->symbol.value == "ARROW" && member->children.size() >= 7) {
                        body = buildExpression(member->children[6].get());
                    }
                }
                
                Type* methodRetType = nullptr;
                if (!returnType.empty()) {
                    // TODO: Implement proper type resolution
                    // For now, check for built-in types
                    if (returnType == "Number") {
                        methodRetType = Type::getNumberType();
                    } else if (returnType == "String") {
                        methodRetType = Type::getStringType();
                    } else if (returnType == "Boolean") {
                        methodRetType = Type::getBooleanType();
                    } else if (returnType == "Object") {
                        methodRetType = Type::getObjectType();
                    }
                    // For user-defined types, leave as nullptr for now
                }
                methods.push_back(new DefFuncNode(memberName, params, body, methodRetType));
            }
        }
        
        // Move to next member
        if (memberList->children.size() > 1) {
            memberList = memberList->children[1].get();
        } else {
            break;
        }
    }
    
    // Convert methods to StatementNode*
    std::vector<StatementNode*> methodStatements;
    for (auto method : methods) {
        methodStatements.push_back(method);
    }
    
    // Create TypeDefNode with appropriate constructor
    // Using the constructor that takes name, type arguments (empty for now), attributes, and methods
    std::vector<Parameter> typeArguments; // TODO: Parse type parameters if needed
    TypeDefNode* typeDefNode = new TypeDefNode(name, typeArguments, attributes, methodStatements);
    
    // Set parent type if specified
    if (!parentType.empty()) {
        typeDefNode->parentTypeName = parentType;
    }
    
    return typeDefNode;
}

StatementNode* ASTBuilder::buildPrintStatement(ParseNode* node) {
    if (!node || node->children.size() < 5) return nullptr;
    
    // PrintStatement -> PRINT LPAREN Expression RPAREN SEMICOLON
    ExpressionNode* expr = buildExpression(node->children[2].get());
    
    return new PrintStatementNode(expr);
}

std::vector<ExpressionNode*> ASTBuilder::buildExpressionList(ParseNode* node) {
    std::vector<ExpressionNode*> expressions;
    
    if (!node || node->symbol.value != "ExpressionList") {
        return expressions;
    }
    
    // ExpressionList -> Expression ExpressionListTail
    if (!node->children.empty()) {
        ExpressionNode* expr = buildExpression(node->children[0].get());
        if (expr) {
            expressions.push_back(expr);
        }
        
        if (node->children.size() > 1) {
            ParseNode* tail = node->children[1].get();
            while (tail && !tail->children.empty()) {
                // ExpressionListTail -> SEMICOLON Expression ExpressionListTail | SEMICOLON | ε
                if (tail->children.size() >= 2 && tail->children[0]->symbol.value == "SEMICOLON") {
                    ExpressionNode* expr = buildExpression(tail->children[1].get());
                    if (expr) {
                        expressions.push_back(expr);
                    }
                    
                    if (tail->children.size() > 2) {
                        tail = tail->children[2].get();
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
    }
    
    return expressions;
}

std::vector<ExpressionNode*> ASTBuilder::buildArgumentList(ParseNode* node) {
    std::vector<ExpressionNode*> arguments;
    
    if (!node || node->symbol.value != "ArgumentList") {
        return arguments;
    }
    
    // ArgumentList -> Expression ArgumentListTail | ε
    if (!node->children.empty()) {
        ExpressionNode* arg = buildExpression(node->children[0].get());
        if (arg) {
            arguments.push_back(arg);
        }
        
        if (node->children.size() > 1) {
            ParseNode* tail = node->children[1].get();
            while (tail && !tail->children.empty()) {
                // ArgumentListTail -> COMMA Expression ArgumentListTail | ε
                if (tail->children.size() >= 3 && tail->children[0]->symbol.value == "COMMA") {
                    ExpressionNode* arg = buildExpression(tail->children[1].get());
                    if (arg) {
                        arguments.push_back(arg);
                    }
                    tail = tail->children[2].get();
                } else {
                    break;
                }
            }
        }
    }
    
    return arguments;
}

std::vector<Parameter> ASTBuilder::buildParameterList(ParseNode* node) {
    std::vector<Parameter> parameters;
    
    if (!node || node->symbol.value != "ParameterList") {
        return parameters;
    }
    
    // ParameterList -> Parameter ParameterListTail | ε
    if (!node->children.empty()) {
        Parameter param = buildParameter(node->children[0].get());
        parameters.push_back(param);
        
        if (node->children.size() > 1) {
            ParseNode* tail = node->children[1].get();
            while (tail && !tail->children.empty()) {
                // ParameterListTail -> COMMA Parameter ParameterListTail | ε
                if (tail->children.size() >= 3 && tail->children[0]->symbol.value == "COMMA") {
                    Parameter param = buildParameter(tail->children[1].get());
                    parameters.push_back(param);
                    tail = tail->children[2].get();
                } else {
                    break;
                }
            }
        }
    }
    
    return parameters;
}

Parameter ASTBuilder::buildParameter(ParseNode* node) {
    if (!node || node->children.empty()) {
        return Parameter("", nullptr);
    }
    
    // Parameter -> IDENTIFIER TypeAnnotationOpt
    std::string name = node->children[0]->token.lexeme;
    Type* type = nullptr;
    
    if (node->children.size() > 1) {
        ParseNode* typeAnnotOpt = node->children[1].get();
        if (typeAnnotOpt && !typeAnnotOpt->children.empty()) {
            ParseNode* typeAnnot = typeAnnotOpt->children[0].get();
            if (typeAnnot && typeAnnot->children.size() >= 2) {
                std::string typeName = typeAnnot->children[1]->token.lexeme;
                // TODO: Implement proper type resolution
                // For now, check for built-in types
                if (typeName == "Number") {
                    type = Type::getNumberType();
                } else if (typeName == "String") {
                    type = Type::getStringType();
                } else if (typeName == "Boolean") {
                    type = Type::getBooleanType();
                } else if (typeName == "Object") {
                    type = Type::getObjectType();
                }
                // For user-defined types, leave as nullptr for now
            }
        }
    }
    
    return Parameter(name, type);
}

std::vector<VarDeclPair> ASTBuilder::buildVarDeclList(ParseNode* node) {
    std::vector<VarDeclPair> declarations;
    
    if (!node || node->symbol.value != "VarDeclList") {
        return declarations;
    }
    
    // VarDeclList -> VarDecl VarDeclListTail
    if (!node->children.empty()) {
        VarDeclPair decl = buildVarDecl(node->children[0].get());
        declarations.push_back(decl);
        
        if (node->children.size() > 1) {
            ParseNode* tail = node->children[1].get();
            while (tail && !tail->children.empty()) {
                // VarDeclListTail -> COMMA VarDecl VarDeclListTail | ε
                if (tail->children.size() >= 3 && tail->children[0]->symbol.value == "COMMA") {
                    VarDeclPair decl = buildVarDecl(tail->children[1].get());
                    declarations.push_back(decl);
                    tail = tail->children[2].get();
                } else {
                    break;
                }
            }
        }
    }
    
    return declarations;
}

VarDeclPair ASTBuilder::buildVarDecl(ParseNode* node) {
    VarDeclPair decl;
    
    if (!node || node->children.size() < 4) {
        return decl;
    }
    
    // VarDecl -> IDENTIFIER TypeAnnotationOpt EQUALS Expression
    decl.id = node->children[0]->token.lexeme;
    
    ParseNode* typeAnnotOpt = node->children[1].get();
    if (typeAnnotOpt && !typeAnnotOpt->children.empty()) {
        ParseNode* typeAnnot = typeAnnotOpt->children[0].get();
        if (typeAnnot && typeAnnot->children.size() >= 2) {
            std::string typeName = typeAnnot->children[1]->token.lexeme;
            // TODO: Implement proper type resolution
            // For now, check for built-in types
            if (typeName == "Number") {
                decl.type = Type::getNumberType();
            } else if (typeName == "String") {
                decl.type = Type::getStringType();
            } else if (typeName == "Boolean") {
                decl.type = Type::getBooleanType();
            } else if (typeName == "Object") {
                decl.type = Type::getObjectType();
            }
            // For user-defined types, leave as nullptr for now
        }
    }
    
    decl.expr = buildExpression(node->children[3].get());
    
    return decl;
}

} // namespace Parser