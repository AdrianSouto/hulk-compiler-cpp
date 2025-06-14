%{
#include "../include/AllHeaders.hpp"
#include "../include/Expressions/ForRangeNode.hpp"
#include "../include/AST/Parameter.hpp"
#include "../include/AST/Attribute.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

int yylex();
void yyerror(const char* s);
int line_count = 1;
Program program;
extern char* yytext;
%}

%token<number> NUMBER
%token<string> STRING IDENTIFIER
%token PRINT
%token EOL
%token LET IN FUNCTION ARROW TYPE INHERITS BASE NEW
%token IF ELIF ELSE IS AS
%token LPAREN RPAREN LBRACE RBRACE
%token TYPE_NUMBER TYPE_STRING TYPE_BOOLEAN
%token TRUE FALSE
%token WHILE FOR
%token PLUS MINUS MULTIPLY DIVIDE MODULO COLON POWER CONCAT CONCAT_SPACE SEMICOLON COMMA EQUALS EQEQ AND GREATEREQ NOTEQ LESS GREATER OR NOT LESSEQ ASSIGN DOT

%union {
    int number;
    char* string;
    ExpressionNode* expression;
    StatementNode* statement;
    ASTNode* ast_node;
    std::vector<ExpressionNode*>* explist;
    std::vector<std::string>* idlist;
    std::vector<VarDeclPair>* decllist;
    std::vector<StatementNode*>* stmtlist;
    std::vector<Attribute>* attrlist;
    std::vector<ASTNode*>* typemembers;
    Type* type;
    Attribute* attribute;
}
%right ASSIGN
%left OR
%left AND
%left EQEQ NOTEQ
%left LESS LESSEQ GREATER GREATEREQ IS AS
%left CONCAT CONCAT_SPACE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right POWER
%right NOT
%left DOT
%left LPAREN RPAREN
%type<expression> expression
%type<expression> primary_expression
%type<expression> postfix_expression
%type<statement> statement
%type<explist> expression_list
%type<idlist> identifier_list
%type<decllist> declaration_list
%type<decllist> declaration
%type<decllist> parameter_list
%type<decllist> parameter
%type<stmtlist> statement_block
%type<stmtlist> statement_list_block
%type<explist> expression_block
%type<typemembers> type_member_list
%type<ast_node> type_member
%type<type> type
%type<ast_node> ast_construct

%%

program:
    statement_list
    | expression SEMICOLON {

        program.Statements.push_back(new ExpressionStatementNode($1));
    }
    | expression {

        program.Statements.push_back(new ExpressionStatementNode($1));
    }
    | { }

    ;

statement_list:
    statement_list statement EOL { program.Statements.push_back($2); }
    | statement_list statement { program.Statements.push_back($2); }
    | statement_list EOL {  }
    | statement EOL { program.Statements.push_back($1); }
    | statement { program.Statements.push_back($1); }
    | EOL { }
    ;

statement:
    PRINT expression SEMICOLON { $$ = new PrintStatementNode($2); }
    | PRINT expression { $$ = new PrintStatementNode($2); }
    | IDENTIFIER ASSIGN expression SEMICOLON { $$ = new ExpressionStatementNode(new AssignmentNode($1, $3)); free($1); }
    | IDENTIFIER ASSIGN expression { $$ = new ExpressionStatementNode(new AssignmentNode($1, $3)); free($1); }
    | LET declaration_list IN statement {
        ASTNode* stmt = $4;
        for (int i = $2->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*$2)[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        $$ = dynamic_cast<StatementNode*>(stmt);
        delete $2;
    }
    | LET declaration_list EOL IN statement {
        ASTNode* stmt = $5;
        for (int i = $2->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*$2)[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        $$ = dynamic_cast<StatementNode*>(stmt);
        delete $2;
    }
    | LET declaration_list IN EOL statement {
        ASTNode* stmt = $5;
        for (int i = $2->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*$2)[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        $$ = dynamic_cast<StatementNode*>(stmt);
        delete $2;
    }
    | LET declaration_list EOL IN EOL statement {
        ASTNode* stmt = $6;
        for (int i = $2->size()-1; i >= 0; i--) {
            VarDeclPair pair = (*$2)[i];
            stmt = new LetVarNode(pair.id.c_str(), pair.expr, stmt, pair.type);
        }
        $$ = dynamic_cast<StatementNode*>(stmt);
        delete $2;
    }
    | LET declaration_list IN expression SEMICOLON {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $4));
        delete $2;
    }
    | LET declaration_list IN expression {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $4));
        delete $2;
    }
    | LET declaration_list EOL IN expression SEMICOLON {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $5));
        delete $2;
    }
    | LET declaration_list EOL IN expression {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $5));
        delete $2;
    }
    | LET declaration_list IN EOL expression SEMICOLON {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $5));
        delete $2;
    }
    | LET declaration_list IN EOL expression {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $5));
        delete $2;
    }
    | LET declaration_list EOL IN EOL expression SEMICOLON {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $6));
        delete $2;
    }
    | LET declaration_list EOL IN EOL expression {
        $$ = new ExpressionStatementNode(new LetExprNode(*$2, $6));
        delete $2;
    }
    | statement_block { $$ = new BlockNode(*$1); delete $1; }
    | FUNCTION IDENTIFIER LPAREN identifier_list RPAREN ARROW expression SEMICOLON {
        $$ = new DefFuncNode($2, *$4, $7);
        delete $4;
    }
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN COLON type ARROW expression SEMICOLON {

        std::vector<Parameter> params;
        for (const auto& pair : *$4) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new DefFuncNode($2, params, $9, $7);
        delete $4;
        free($2);
    }
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN ARROW expression SEMICOLON {

        std::vector<Parameter> params;
        for (const auto& pair : *$4) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new DefFuncNode($2, params, $7, nullptr);
        delete $4;
        free($2);
    }
    | FUNCTION IDENTIFIER LPAREN identifier_list RPAREN LBRACE statement_list_block RBRACE {
        std::vector<Parameter> params;
        for (const auto& id : *$4) {
            params.emplace_back(id, nullptr);
        }
        $$ = new DefFuncNode($2, params, *$7, nullptr);
        delete $4;
        delete $7;
        free($2);
    }
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN LBRACE statement_list_block RBRACE {
        std::vector<Parameter> params;
        for (const auto& pair : *$4) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new DefFuncNode($2, params, *$7, nullptr);
        delete $4;
        delete $7;
        free($2);
    }
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN COLON type LBRACE statement_list_block RBRACE {
        std::vector<Parameter> params;
        for (const auto& pair : *$4) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new DefFuncNode($2, params, *$9, $7);
        delete $4;
        delete $9;
        free($2);
    }
    | FOR LPAREN IDENTIFIER IN expression RPAREN ast_construct SEMICOLON {

        if (auto* funcCall = dynamic_cast<FuncCallNode*>($5)) {
            if (funcCall->identifier == "range" && funcCall->args.size() == 2) {

                ForRangeNode* forNode = new ForRangeNode($3, funcCall->args[0], funcCall->args[1], $7);
                $$ = new ExpressionStatementNode(forNode);
                funcCall->args.clear();
                delete funcCall;
                free($3);
            } else {

                $$ = new ExpressionStatementNode(new NumberNode(0));
                free($3);
            }
        } else {
            $$ = new ExpressionStatementNode(new NumberNode(0));
            free($3);
        }
    }
    | FOR LPAREN IDENTIFIER IN expression RPAREN ast_construct {

        if (auto* funcCall = dynamic_cast<FuncCallNode*>($5)) {
            if (funcCall->identifier == "range" && funcCall->args.size() == 2) {

                ForRangeNode* forNode = new ForRangeNode($3, funcCall->args[0], funcCall->args[1], $7);
                $$ = new ExpressionStatementNode(forNode);
                funcCall->args.clear();
                delete funcCall;
                free($3);
            } else {

                $$ = new ExpressionStatementNode(new NumberNode(0));
                free($3);
            }
        } else {
            $$ = new ExpressionStatementNode(new NumberNode(0));
            free($3);
        }
    }
    | TYPE IDENTIFIER LPAREN parameter_list RPAREN LBRACE type_member_list RBRACE {

        std::vector<Parameter> params;
        for (const auto& pair : *$4) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new TypeDefNode($2, params, *$7);
        delete $4;
        delete $7;
        free($2);
    }
    | TYPE IDENTIFIER LPAREN RPAREN LBRACE type_member_list RBRACE {

        std::vector<Parameter> params;
        $$ = new TypeDefNode($2, params, *$6);
        delete $6;
        free($2);
    }
    | TYPE IDENTIFIER LBRACE type_member_list RBRACE {

        std::vector<Parameter> params;
        $$ = new TypeDefNode($2, params, *$4);
        delete $4;
        free($2);
    }
    | TYPE IDENTIFIER INHERITS IDENTIFIER LBRACE type_member_list RBRACE {

        std::vector<Parameter> params;
        std::vector<ExpressionNode*> parentArgs;
        $$ = new TypeDefNode($2, params, $4, parentArgs, *$6);
        delete $6;
        free($2);
        free($4);
    }
    | TYPE IDENTIFIER LPAREN parameter_list RPAREN INHERITS IDENTIFIER LBRACE type_member_list RBRACE {

        std::vector<Parameter> params;
        for (const auto& pair : *$4) {
            params.emplace_back(pair.id, pair.type);
        }
        std::vector<ExpressionNode*> parentArgs;
        $$ = new TypeDefNode($2, params, $7, parentArgs, *$9);
        delete $4;
        delete $9;
        free($2);
        free($7);
    }
    | TYPE IDENTIFIER LPAREN parameter_list RPAREN INHERITS IDENTIFIER LPAREN expression_list RPAREN LBRACE type_member_list RBRACE {

        std::vector<Parameter> params;
        for (const auto& pair : *$4) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new TypeDefNode($2, params, $7, *$9, *$12);
        delete $4;
        delete $9;
        delete $12;
        free($2);
        free($7);
    }
    | TYPE IDENTIFIER INHERITS IDENTIFIER LPAREN expression_list RPAREN LBRACE type_member_list RBRACE {

        std::vector<Parameter> params;
        $$ = new TypeDefNode($2, params, $4, *$6, *$9);
        delete $6;
        delete $9;
        free($2);
        free($4);
    }
    ;

statement_block:
    LBRACE statement_list_block RBRACE { $$ = $2; }
    ;

statement_list_block:
    statement {
        $$ = new std::vector<StatementNode*>;
        $$->push_back($1);
    }
    | statement_list_block statement {
        $1->push_back($2);
        $$ = $1;
    }
    | statement_list_block EOL statement {
        $1->push_back($3);
        $$ = $1;
    }
    | statement_list_block EOL { $$ = $1; }
    | EOL { $$ = new std::vector<StatementNode*>; }
    |  { $$ = new std::vector<StatementNode*>; }
    ;

declaration_list:
    declaration { $$ = $1; }
    | declaration_list COMMA EOL declaration {
        $$ = $1;
        for (const auto& pair : *$4) {
            $$->push_back(pair);
        }
        delete $4;
    }
    | declaration_list COMMA declaration {
        $$ = $1;
        for (const auto& pair : *$3) {
            $$->push_back(pair);
        }
        delete $3;
    }
    | declaration_list EOL declaration {
        $$ = $1;
        for (const auto& pair : *$3) {
            $$->push_back(pair);
        }
        delete $3;
    }
    ;

declaration:
    IDENTIFIER EQUALS expression {
        $$ = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = $1;
        pair.expr = $3;
        pair.type = Type::getUnknownType();
        $$->push_back(pair);
        free($1);
    }
    | IDENTIFIER COLON type EQUALS expression {
        $$ = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = $1;
        pair.expr = $5;
        pair.type = $3;
        $$->push_back(pair);
        free($1);
    }
    ;

type:
    TYPE_NUMBER { $$ = Type::getNumberType(); }
    | TYPE_STRING { $$ = Type::getStringType(); }
    | TYPE_BOOLEAN { $$ = Type::getBooleanType(); }
    | IDENTIFIER {

        Type* foundType = getTypeByName($1);
        $$ = foundType;
        free($1);
    }
    ;

expression:
    postfix_expression { $$ = $1; }
    | expression PLUS expression { $$ = new AdditionNode($1, $3); }
    | expression MINUS expression { $$ = new SubtractionNode($1, $3); }
    | expression MULTIPLY expression { $$ = new MultiplicationNode($1, $3); }
    | expression DIVIDE expression { $$ = new DivisionNode($1, $3); }
    | expression MODULO expression { $$ = new ModuloNode($1, $3); }
    | expression POWER expression { $$ = new PowerNode($1, $3); }
    | expression CONCAT expression { $$ = new ConcatenationNode($1, $3); }
    | expression CONCAT_SPACE expression {
        ExpressionNode* space = new StringLiteralNode(strdup(" "));
        ExpressionNode* leftConcat = new ConcatenationNode($1, space);
        $$ = new ConcatenationNode(leftConcat, $3);
    }
    | expression EQEQ expression { $$ = new EqualNode($1, $3); }
    | expression NOTEQ expression { $$ = new NotEqualNode($1, $3); }
    | expression LESS expression { $$ = new LessNode($1, $3); }
    | expression LESSEQ expression { $$ = new LessEqNode($1, $3); }
    | expression GREATER expression { $$ = new GreaterNode($1, $3); }
    | expression GREATEREQ expression { $$ = new GreaterEqNode($1, $3); }
    | expression IS IDENTIFIER { $$ = new IsNode($1, $3); free($3); }
    | expression IS TYPE_NUMBER { $$ = new IsNode($1, "Number"); }
    | expression IS TYPE_STRING { $$ = new IsNode($1, "String"); }
    | expression IS TYPE_BOOLEAN { $$ = new IsNode($1, "Boolean"); }
    | expression IS TRUE { $$ = new IsNode($1, "Boolean"); }
    | expression IS FALSE { $$ = new IsNode($1, "Boolean"); }
    | expression AS IDENTIFIER { $$ = new AsNode($1, $3); free($3); }
    | expression AND expression { $$ = new AndNode($1, $3); }
    | expression OR expression { $$ = new OrNode($1, $3); }
    | NOT expression { $$ = new NotNode($2); }
    | IDENTIFIER ASSIGN expression { $$ = new AssignmentNode($1, $3); free($1); }
    | postfix_expression ASSIGN expression { 
        if (auto memberAccess = dynamic_cast<MemberAccessNode*>($1)) {
            $$ = new MemberAssignmentNode(memberAccess->object, memberAccess->memberName, $3);
            delete memberAccess;
        } else if (auto selfMemberAccess = dynamic_cast<SelfMemberAccessNode*>($1)) {
            $$ = new SelfMemberAssignmentNode(selfMemberAccess->attributeName, $3);
            delete selfMemberAccess;
        } else {
            $$ = new AssignmentNode("", $3);
        }
    }
    | IF expression ast_construct ELSE ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->setElse($5);
            $$ = condNode ;
        }
    | IF expression ast_construct EOL ELSE ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->setElse($6);
            $$ = condNode ;
        }
    | IF expression ast_construct ELSE EOL ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->setElse($6);
            $$ = condNode ;
        }
    | IF expression ast_construct EOL ELSE EOL ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->setElse($7);
            $$ = condNode ;
        }
    | IF expression ast_construct ELIF expression ast_construct ELSE ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->addBranch($5, $6);
            condNode->setElse($8);
            $$ = condNode ;
        }
    | IF expression ast_construct ELIF expression ast_construct EOL ELSE ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->addBranch($5, $6);
            condNode->setElse($9);
            $$ = condNode ;
        }
    | IF expression ast_construct ELIF expression ast_construct ELSE EOL ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->addBranch($5, $6);
            condNode->setElse($9);
            $$ = condNode ;
        }
    | IF expression ast_construct EOL ELIF expression ast_construct ELSE ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->addBranch($6, $7);
            condNode->setElse($9);
            $$ = condNode ;
        }
    | IF expression ast_construct EOL ELIF expression ast_construct EOL ELSE ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->addBranch($6, $7);
            condNode->setElse($10);
            $$ = condNode ;
        }
    | IF expression ast_construct EOL ELIF expression ast_construct ELSE EOL ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->addBranch($6, $7);
            condNode->setElse($10);
            $$ = condNode ;
        }
    | IF expression ast_construct EOL ELIF expression ast_construct EOL ELSE EOL ast_construct {
            ConditionalNode* condNode = new ConditionalNode();
            condNode->addBranch($2, $3);
            condNode->addBranch($6, $7);
            condNode->setElse($11);
            $$ = condNode ;
        }

    | LET declaration_list IN expression { $$ = new LetExprNode(*$2, $4); delete $2; }
    | LET declaration_list EOL IN expression { $$ = new LetExprNode(*$2, $5); delete $2; }
    | LET declaration_list IN EOL expression { $$ = new LetExprNode(*$2, $5); delete $2; }
    | LET declaration_list EOL IN EOL expression { $$ = new LetExprNode(*$2, $6); delete $2; }
    | WHILE LPAREN expression RPAREN ast_construct { $$ = new WhileNode($3, $5); }
    | WHILE expression ast_construct { $$ = new WhileNode($2, $3); }
    | FOR LPAREN IDENTIFIER IN expression RPAREN ast_construct {

        if (auto* funcCall = dynamic_cast<FuncCallNode*>($5)) {
            if (funcCall->identifier == "range" && funcCall->args.size() == 2) {

                $$ = new ForRangeNode($3, funcCall->args[0], funcCall->args[1], $7);

                funcCall->args.clear();
                delete funcCall;
                free($3);
            } else {


                std::string iterableVar = std::string("_iterable_") + $3;

                ExpressionNode* nextCall = new FuncCallNode("next", {new VariableNode(iterableVar.c_str())});
                ExpressionNode* currentCall = new FuncCallNode("current", {new VariableNode(iterableVar.c_str())});

                std::vector<VarDeclPair> innerDecls;
                VarDeclPair innerPair;
                innerPair.id = $3;
                innerPair.expr = currentCall;
                innerPair.type = Type::getUnknownType();
                innerDecls.push_back(innerPair);

                ASTNode* letBody = nullptr;
                if (dynamic_cast<ExpressionNode*>($7)) {
                    letBody = new LetExprNode(innerDecls, dynamic_cast<ExpressionNode*>($7));
                } else {
                    letBody = new LetVarNode($3, currentCall, $7, Type::getUnknownType());
                }

                WhileNode* whileLoop = new WhileNode(nextCall, letBody);

                std::vector<VarDeclPair> outerDecls;
                VarDeclPair outerPair;
                outerPair.id = iterableVar;
                outerPair.expr = $5;
                outerPair.type = Type::getUnknownType();
                outerDecls.push_back(outerPair);

                $$ = new LetExprNode(outerDecls, whileLoop);
                free($3);
            }
        } else {

            std::string iterableVar = std::string("_iterable_") + $3;

            ExpressionNode* nextCall = new FuncCallNode("next", {new VariableNode(iterableVar.c_str())});
            ExpressionNode* currentCall = new FuncCallNode("current", {new VariableNode(iterableVar.c_str())});

            std::vector<VarDeclPair> innerDecls;
            VarDeclPair innerPair;
            innerPair.id = $3;
            innerPair.expr = currentCall;
            innerPair.type = Type::getUnknownType();
            innerDecls.push_back(innerPair);

            ASTNode* letBody = nullptr;
            if (dynamic_cast<ExpressionNode*>($7)) {
                letBody = new LetExprNode(innerDecls, dynamic_cast<ExpressionNode*>($7));
            } else {
                letBody = new LetVarNode($3, currentCall, $7, Type::getUnknownType());
            }

            WhileNode* whileLoop = new WhileNode(nextCall, letBody);

            std::vector<VarDeclPair> outerDecls;
            VarDeclPair outerPair;
            outerPair.id = iterableVar;
            outerPair.expr = $5;
            outerPair.type = Type::getUnknownType();
            outerDecls.push_back(outerPair);

            $$ = new LetExprNode(outerDecls, whileLoop);
            free($3);
        }
    }
    | LBRACE expression_block RBRACE {
        $$ = new BlockExprNode(*$2);
        delete $2;
    }
    ;

postfix_expression:
    primary_expression { $$ = $1; }
    | postfix_expression DOT IDENTIFIER {

        if (auto varNode = dynamic_cast<VariableNode*>($1)) {
            if (varNode->identifier == "self") {

                $$ = new SelfMemberAccessNode($3);
                delete varNode;
            } else {
                $$ = new MemberAccessNode($1, $3);
            }
        } else {
            $$ = new MemberAccessNode($1, $3);
        }
        free($3);
    }
    | postfix_expression DOT IDENTIFIER LPAREN expression_list RPAREN {
        $$ = new MethodCallNode($1, $3, *$5);
        delete $5;
        free($3);
    }
    | postfix_expression DOT IDENTIFIER LPAREN RPAREN {
        $$ = new MethodCallNode($1, $3, {});
        free($3);
    }
    | IDENTIFIER LPAREN expression_list RPAREN { $$ = new FuncCallNode($1, *$3); delete $3; }
    | IDENTIFIER LPAREN RPAREN { $$ = new FuncCallNode($1, {}); }
    ;

primary_expression:
    NUMBER { $$ = new NumberNode($1); }
    | STRING { $$ = new StringLiteralNode($1); }
    | TRUE { $$ = new BooleanNode(true); }
    | FALSE { $$ = new BooleanNode(false); }
    | IDENTIFIER { $$ = new VariableNode($1); }
    | LPAREN expression RPAREN { $$ = $2; }
    | NEW IDENTIFIER LPAREN expression_list RPAREN {
        $$ = new TypeInstantiationNode($2, *$4);
        delete $4;
        free($2);
    }
    | NEW IDENTIFIER LPAREN RPAREN {
        $$ = new TypeInstantiationNode($2, {});
        free($2);
    }
    | BASE LPAREN expression_list RPAREN {
        $$ = new BaseCallNode(*$3);
        delete $3;
    }
    | BASE LPAREN RPAREN {
        $$ = new BaseCallNode({});
    }
    ;

expression_list:
    expression { $$ = new std::vector<ExpressionNode*>; $$->push_back($1); }
    | expression_list COMMA EOL expression { $1->push_back($4); $$ = $1; }
    | expression_list COMMA expression { $1->push_back($3); $$ = $1; }
    | expression_list EOL expression { $1->push_back($3); $$ = $1; }
    ;

identifier_list:
    IDENTIFIER { $$ = new std::vector<std::string>; $$->push_back($1); }
    | identifier_list COMMA EOL IDENTIFIER { $1->push_back($4); $$ = $1; }
    | identifier_list COMMA IDENTIFIER { $1->push_back($3); $$ = $1; }
    | identifier_list EOL IDENTIFIER { $1->push_back($3); $$ = $1; }
    |  { $$ = new std::vector<std::string>; }
    ;

parameter_list:
    parameter { $$ = $1; }
    | parameter_list COMMA EOL parameter {
        $$ = $1;
        for (const auto& pair : *$4) {
            $$->push_back(pair);
        }
        delete $4;
    }
    | parameter_list COMMA parameter {
        $$ = $1;
        for (const auto& pair : *$3) {
            $$->push_back(pair);
        }
        delete $3;
    }
    | parameter_list EOL parameter {
        $$ = $1;
        for (const auto& pair : *$3) {
            $$->push_back(pair);
        }
        delete $3;
    }
    |  { $$ = new std::vector<VarDeclPair>; }
    ;

parameter:
    IDENTIFIER {
        $$ = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = $1;
        pair.expr = nullptr;
        pair.type = nullptr;
        $$->push_back(pair);
        free($1);
    }
    | IDENTIFIER COLON type {
        $$ = new std::vector<VarDeclPair>;
        VarDeclPair pair;
        pair.id = $1;
        pair.expr = nullptr;
        pair.type = $3;
        $$->push_back(pair);
        free($1);
    }
    ;

ast_construct:
    expression { $$ = $1; }
    | statement  { $$ = $1; }
    ;

expression_block:
    expression {
        $$ = new std::vector<ExpressionNode*>;
        $$->push_back($1);
    }
    | expression SEMICOLON {
        $$ = new std::vector<ExpressionNode*>;
        $$->push_back($1);
    }
    | expression_block expression {
        $1->push_back($2);
        $$ = $1;
    }
    | expression_block expression SEMICOLON {
        $1->push_back($2);
        $$ = $1;
    }
    | expression_block EOL {
        $$ = $1;
    }
    | EOL {
        $$ = new std::vector<ExpressionNode*>;
    }
    | {
        $$ = new std::vector<ExpressionNode*>;
    }
    ;


type_member_list:
    { $$ = new std::vector<ASTNode*>; }
    | type_member_list type_member {
        $1->push_back($2);
        $$ = $1;
    }
    ;

type_member:
    IDENTIFIER EQUALS expression SEMICOLON {
        $$ = new Attribute($1, nullptr, $3);
        free($1);
    }
    | IDENTIFIER COLON type EQUALS expression SEMICOLON {
        $$ = new Attribute($1, $3, $5);
        free($1);
    }
    | IDENTIFIER LPAREN parameter_list RPAREN ARROW expression SEMICOLON {
        std::vector<Parameter> params;
        for (const auto& pair : *$3) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new DefFuncNode($1, params, $6, nullptr);
        delete $3;
        free($1);
    }
    | IDENTIFIER LPAREN RPAREN ARROW expression SEMICOLON {
        std::vector<Parameter> params;
        $$ = new DefFuncNode($1, params, $5, nullptr);
        free($1);
    }
    | IDENTIFIER LPAREN parameter_list RPAREN COLON type ARROW expression SEMICOLON {
        std::vector<Parameter> params;
        for (const auto& pair : *$3) {
            params.emplace_back(pair.id, pair.type);
        }
        $$ = new DefFuncNode($1, params, $8, $6);
        delete $3;
        free($1);
    }
    | IDENTIFIER LPAREN RPAREN COLON type ARROW expression SEMICOLON {
        std::vector<Parameter> params;
        $$ = new DefFuncNode($1, params, $7, $5);
        free($1);
    }
    ;

%%

void yyerror(const char* s) {
    std::cerr << "Error: " << s << " en línea " << line_count << ", cerca de '" << yytext << "'" << std::endl;
}
