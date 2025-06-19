%{
#include "../include/AllHeaders.hpp"
#include <iostream>
#include <string>
#include <vector>

// Declaraciones externas
int yylex();
void yyerror(const char* s);
extern int yylineno;  // Variable del lexer que mantiene el número de línea actual
Program program;  // Cambiado de Program* a Program para compatibilidad con main.cpp
extern char* yytext;
%}

/* Definición de la unión para los valores semánticos */
%union {
    double number;
    double decimal;  // Agregado para compatibilidad con el lexer
    char* string;

    // Nodos del AST
    Program* program_node;
    StatementNode* statement;
    ExpressionNode* expression;

    // Listas y vectores
    std::vector<StatementNode*>* statement_list;
    std::vector<ExpressionNode*>* expression_list;
    std::vector<Parameter>* parameter_list;
    std::vector<VarDeclPair>* var_decl_list;
    std::vector<std::string>* string_list;

    // Tipos auxiliares
    Parameter* parameter;
    VarDeclPair* var_decl;
    std::vector<std::pair<ExpressionNode*, ExpressionNode*>>* elif_list;
    
    // Tipos para atributos
    std::vector<Attribute>* attribute_list;
    Attribute* attribute;
    Type* type_node;
    
    // Tipos para miembros de tipos (atributos y métodos)
    std::vector<ASTNode*>* member_list;
    ASTNode* member;
}

/* Tokens */
%token<number> NUMBER
%token<string> STRING IDENTIFIER
%token PRINT
%token LET IN FUNCTION ARROW TYPE INHERITS NEW
%token IF ELSE ELIF
%token WHILE FOR
%token PLUS MINUS MULTIPLY DIVIDE MODULO COLON POWER CONCAT CONCAT_SPACE SEMICOLON COMMA EQUALS EQEQ AND GREATEREQ NOTEQ LESS GREATER OR NOT LESSEQ ASSIGN DOT
%token LPAREN RPAREN LBRACE RBRACE
%token TRUE FALSE
%token BASE IS AS
%token TYPE_NUMBER TYPE_STRING TYPE_BOOLEAN
/* Constantes matemáticas */
%token PI E
/* Funciones matemáticas builtin */
%token SQRT SIN COS EXP LOG RAND

/* Precedencias y asociatividad (de menor a mayor precedencia) */
%right ASSIGN
%left OR
%left AND
%nonassoc EQEQ NOTEQ
%nonassoc LESS LESSEQ GREATER GREATEREQ
%nonassoc IS AS
%left CONCAT CONCAT_SPACE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right POWER
%right UMINUS NOT
%left DOT
%nonassoc LPAREN RPAREN

/* Tipos de los no-terminales */
%type<program_node> program
%type<statement_list> declaration_list statement_list
%type<statement> declaration type_definition function_definition statement
%type<expression> expression primary_expression
%type<expression_list> expression_list expression_list_opt expression_sequence
%type<string_list> identifier_list
%type<var_decl_list> var_decl_list
%type<elif_list> elif_list
%type<var_decl> var_decl
%type<parameter_list> parameter_list
%type<parameter> parameter
%type<attribute_list> attribute_list
%type<attribute> attribute
%type<type_node> type_annotation
%type<member_list> member_list
%type<member> member

%%

/* Regla principal del programa */
program:
    /* Solo una expresión global */
    expression {
        program.Statements.clear();
        program.Statements.push_back(new ExpressionStatementNode($1));
    }
    /* Múltiples expresiones globales - todas deben terminar con punto y coma */
    | expression_sequence {
        program.Statements.clear();
        for (auto expr : *$1) {
            program.Statements.push_back(new ExpressionStatementNode(expr));
        }
        delete $1;
    }
    /* Declaraciones seguidas de expresión global */
    | declaration_list expression {
        program.Statements.clear();
        program.Statements = *$1;
        program.Statements.push_back(new ExpressionStatementNode($2));
        delete $1;
    }
    /* Declaraciones seguidas de múltiples expresiones */
    | declaration_list expression_sequence {
        program.Statements.clear();
        program.Statements = *$1;
        for (auto expr : *$2) {
            program.Statements.push_back(new ExpressionStatementNode(expr));
        }
        delete $1;
        delete $2;
    }
    /* Lista mixta de statements */
    | statement_list {
        program.Statements.clear();
        program.Statements = *$1;
        delete $1;
    }
    ;

/* Lista de statements (declaraciones y expresiones mezcladas) */
statement_list:
    statement {
        $$ = new std::vector<StatementNode*>();
        $$->push_back($1);
    }
    | statement_list statement {
        $$ = $1;
        $$->push_back($2);
    }
    ;

/* Statement individual */
statement:
    declaration
    | expression SEMICOLON {
        $$ = new ExpressionStatementNode($1);
    }
    ;

/* Lista de declaraciones (tipos y funciones) */
declaration_list:
    declaration {
        $$ = new std::vector<StatementNode*>();
        $$->push_back($1);
    }
    | declaration_list declaration {
        $$ = $1;
        $$->push_back($2);
    }
    ;

/* Declaraciones (solo tipos y funciones) */
declaration:
    type_definition
    | function_definition
    ;

/* Definición de tipos */
type_definition:
    TYPE IDENTIFIER LBRACE RBRACE {
        // Tipo vacío sin miembros
        $$ = new TypeDefNode($2, std::vector<Parameter>(), std::vector<ASTNode*>());
        free($2);
    }
    | TYPE IDENTIFIER LBRACE member_list RBRACE {
        // Tipo con miembros (atributos y métodos)
        $$ = new TypeDefNode($2, std::vector<Parameter>(), *$4);
        free($2);
        delete $4;
    }
    | TYPE IDENTIFIER LPAREN RPAREN LBRACE RBRACE {
        // Tipo con parámetros vacíos sin miembros
        $$ = new TypeDefNode($2, std::vector<Parameter>(), std::vector<ASTNode*>());
        free($2);
    }
    | TYPE IDENTIFIER LPAREN RPAREN LBRACE member_list RBRACE {
        // Tipo con parámetros vacíos con miembros
        $$ = new TypeDefNode($2, std::vector<Parameter>(), *$6);
        free($2);
        delete $6;
    }
    | TYPE IDENTIFIER LPAREN parameter_list RPAREN LBRACE RBRACE {
        // Tipo con parámetros tipados sin miembros
        $$ = new TypeDefNode($2, *$4, std::vector<ASTNode*>());
        free($2);
        delete $4;
    }
    | TYPE IDENTIFIER LPAREN parameter_list RPAREN LBRACE member_list RBRACE {
        // Tipo con parámetros tipados con miembros
        $$ = new TypeDefNode($2, *$4, *$7);
        free($2);
        delete $4;
        delete $7;
    }
    /* Tipos con herencia */
    | TYPE IDENTIFIER INHERITS IDENTIFIER LBRACE RBRACE {
        // Tipo que hereda sin parámetros propios, sin miembros
        $$ = new TypeDefNode($2, std::vector<Parameter>(), $4, std::vector<ExpressionNode*>(), std::vector<ASTNode*>());
        free($2);
        free($4);
    }
    | TYPE IDENTIFIER INHERITS IDENTIFIER LBRACE member_list RBRACE {
        // Tipo que hereda sin parámetros propios, con miembros
        $$ = new TypeDefNode($2, std::vector<Parameter>(), $4, std::vector<ExpressionNode*>(), *$6);
        free($2);
        free($4);
        delete $6;
    }
    | TYPE IDENTIFIER LPAREN RPAREN INHERITS IDENTIFIER LPAREN expression_list_opt RPAREN LBRACE RBRACE {
        // Tipo con parámetros vacíos que hereda con argumentos para el padre, sin miembros
        $$ = new TypeDefNode($2, std::vector<Parameter>(), $6, *$8, std::vector<ASTNode*>());
        free($2);
        free($6);
        delete $8;
    }
    | TYPE IDENTIFIER LPAREN RPAREN INHERITS IDENTIFIER LPAREN expression_list_opt RPAREN LBRACE member_list RBRACE {
        // Tipo con parámetros vacíos que hereda con argumentos para el padre, con miembros
        $$ = new TypeDefNode($2, std::vector<Parameter>(), $6, *$8, *$11);
        free($2);
        free($6);
        delete $8;
        delete $11;
    }
    | TYPE IDENTIFIER LPAREN parameter_list RPAREN INHERITS IDENTIFIER LPAREN expression_list_opt RPAREN LBRACE RBRACE {
        // Tipo con parámetros que hereda con argumentos para el padre, sin miembros
        $$ = new TypeDefNode($2, *$4, $7, *$9, std::vector<ASTNode*>());
        free($2);
        delete $4;
        free($7);
        delete $9;
    }
    | TYPE IDENTIFIER LPAREN parameter_list RPAREN INHERITS IDENTIFIER LPAREN expression_list_opt RPAREN LBRACE member_list RBRACE {
        // Tipo con parámetros que hereda con argumentos para el padre, con miembros
        $$ = new TypeDefNode($2, *$4, $7, *$9, *$12);
        free($2);
        delete $4;
        free($7);
        delete $9;
        delete $12;
    }
    ;

/* Lista de miembros (atributos y métodos) */
member_list:
    member {
        $$ = new std::vector<ASTNode*>();
        $$->push_back($1);
    }
    | member_list member {
        $$ = $1;
        $$->push_back($2);
    }
    ;

/* Miembro individual (atributo o método) */
member:
    /* Atributos */
    IDENTIFIER COLON type_annotation EQUALS expression SEMICOLON {
        $$ = new Attribute($1, $3, $5);
        free($1);
    }
    | IDENTIFIER COLON type_annotation SEMICOLON {
        $$ = new Attribute($1, $3, nullptr);
        free($1);
    }
    | IDENTIFIER EQUALS expression SEMICOLON {
        $$ = new Attribute($1, nullptr, $3);
        free($1);
    }
    | IDENTIFIER SEMICOLON {
        $$ = new Attribute($1, nullptr, nullptr);
        free($1);
    }
    /* Métodos - sin parámetros con tipo de retorno */
    | IDENTIFIER LPAREN RPAREN COLON type_annotation ARROW expression SEMICOLON {
        // Crear un DefFuncNode con self como primer parámetro implícito
        std::vector<Parameter> params;
        params.emplace_back("self", nullptr);  // self sin tipo explícito
        DefFuncNode* func = new DefFuncNode($1, params, $7);
        func->returnType = $5;
        $$ = func;
        free($1);
    }
    /* Métodos - con parámetros y tipo de retorno */
    | IDENTIFIER LPAREN parameter_list RPAREN COLON type_annotation ARROW expression SEMICOLON {
        // Agregar self como primer parámetro
        std::vector<Parameter> params;
        params.emplace_back("self", nullptr);  // self sin tipo explícito
        params.insert(params.end(), $3->begin(), $3->end());
        DefFuncNode* func = new DefFuncNode($1, params, $8);
        func->returnType = $6;
        $$ = func;
        free($1);
        delete $3;
    }
    /* Métodos - sin parámetros sin tipo de retorno */
    | IDENTIFIER LPAREN RPAREN ARROW expression SEMICOLON {
        // Crear un DefFuncNode con self como primer parámetro implícito
        std::vector<Parameter> params;
        params.emplace_back("self", nullptr);  // self sin tipo explícito
        $$ = new DefFuncNode($1, params, $5);
        free($1);
    }
    /* Métodos - con parámetros sin tipo de retorno */
    | IDENTIFIER LPAREN parameter_list RPAREN ARROW expression SEMICOLON {
        // Agregar self como primer parámetro
        std::vector<Parameter> params;
        params.emplace_back("self", nullptr);  // self sin tipo explícito
        params.insert(params.end(), $3->begin(), $3->end());
        $$ = new DefFuncNode($1, params, $6);
        free($1);
        delete $3;
    }
    ;

/* Lista de atributos */
attribute_list:
    attribute {
        $$ = new std::vector<Attribute>();
        $$->push_back(*$1);
        delete $1;
    }
    | attribute_list attribute {
        $$ = $1;
        $$->push_back(*$2);
        delete $2;
    }
    ;

/* Atributo individual */
attribute:
    IDENTIFIER COLON type_annotation EQUALS expression SEMICOLON {
        $$ = new Attribute($1, $3, $5);
        free($1);
    }
    | IDENTIFIER COLON type_annotation SEMICOLON {
        $$ = new Attribute($1, $3, nullptr);
        free($1);
    }
    | IDENTIFIER EQUALS expression SEMICOLON {
        $$ = new Attribute($1, nullptr, $3);
        free($1);
    }
    | IDENTIFIER SEMICOLON {
        $$ = new Attribute($1, nullptr, nullptr);
        free($1);
    }
    ;

/* Anotación de tipo */
type_annotation:
    TYPE_NUMBER {
        $$ = Type::getNumberType();
    }
    | TYPE_STRING {
        $$ = Type::getStringType();
    }
    | TYPE_BOOLEAN {
        $$ = Type::getBooleanType();
    }
    | IDENTIFIER {
        // Tipo definido por el usuario
        $$ = Type::createUserDefinedType($1, nullptr);
        free($1);
    }
    ;

/* Definición de función */
function_definition:
    /* Función inline sin parámetros sin tipo de retorno */
    FUNCTION IDENTIFIER LPAREN RPAREN ARROW expression SEMICOLON {
        $$ = new DefFuncNode($2, std::vector<std::string>(), $6);
        free($2);
    }
    /* Función inline sin parámetros con tipo de retorno */
    | FUNCTION IDENTIFIER LPAREN RPAREN COLON type_annotation ARROW expression SEMICOLON {
        DefFuncNode* func = new DefFuncNode($2, std::vector<std::string>(), $8);
        func->returnType = $6;
        $$ = func;
        free($2);
    }
    /* Función inline con parámetros sin tipo de retorno (usando identifier_list) */
    | FUNCTION IDENTIFIER LPAREN identifier_list RPAREN ARROW expression SEMICOLON {
        $$ = new DefFuncNode($2, *$4, $7);
        free($2);
        delete $4;
    }
    /* Función inline con parámetros tipados sin tipo de retorno */
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN ARROW expression SEMICOLON {
        $$ = new DefFuncNode($2, *$4, $7);
        free($2);
        delete $4;
    }
    /* Función inline con parámetros tipados y tipo de retorno */
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN COLON type_annotation ARROW expression SEMICOLON {
        DefFuncNode* func = new DefFuncNode($2, *$4, $9);
        func->returnType = $7;
        $$ = func;
        free($2);
        delete $4;
    }
    /* Función con bloque sin parámetros sin tipo de retorno */
    | FUNCTION IDENTIFIER LPAREN RPAREN LBRACE expression_sequence RBRACE {
        // Convertir expresiones a statements
        std::vector<StatementNode*> stmts;
        for (auto expr : *$6) {
            stmts.push_back(new ExpressionStatementNode(expr));
        }
        $$ = new DefFuncNode($2, std::vector<Parameter>(), stmts);
        free($2);
        delete $6;
    }
    /* Función con bloque sin parámetros con tipo de retorno */
    | FUNCTION IDENTIFIER LPAREN RPAREN COLON type_annotation LBRACE expression_sequence RBRACE {
        // Convertir expresiones a statements
        std::vector<StatementNode*> stmts;
        for (auto expr : *$8) {
            stmts.push_back(new ExpressionStatementNode(expr));
        }
        DefFuncNode* func = new DefFuncNode($2, std::vector<Parameter>(), stmts);
        func->returnType = $6;
        $$ = func;
        free($2);
        delete $8;
    }
    /* Función con bloque con parámetros sin tipo de retorno (usando identifier_list) */
    | FUNCTION IDENTIFIER LPAREN identifier_list RPAREN LBRACE expression_sequence RBRACE {
        // Convertir expresiones a statements
        std::vector<StatementNode*> stmts;
        for (auto expr : *$7) {
            stmts.push_back(new ExpressionStatementNode(expr));
        }
        // Convertir strings a Parameters
        std::vector<Parameter> params;
        for (const auto& paramName : *$4) {
            params.emplace_back(paramName, nullptr);
        }
        $$ = new DefFuncNode($2, params, stmts);
        free($2);
        delete $4;
        delete $7;
    }
    /* Función con bloque con parámetros tipados sin tipo de retorno */
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN LBRACE expression_sequence RBRACE {
        // Convertir expresiones a statements
        std::vector<StatementNode*> stmts;
        for (auto expr : *$7) {
            stmts.push_back(new ExpressionStatementNode(expr));
        }
        $$ = new DefFuncNode($2, *$4, stmts);
        free($2);
        delete $4;
        delete $7;
    }
    /* Función con bloque con parámetros tipados y tipo de retorno */
    | FUNCTION IDENTIFIER LPAREN parameter_list RPAREN COLON type_annotation LBRACE expression_sequence RBRACE {
        // Convertir expresiones a statements
        std::vector<StatementNode*> stmts;
        for (auto expr : *$9) {
            stmts.push_back(new ExpressionStatementNode(expr));
        }
        DefFuncNode* func = new DefFuncNode($2, *$4, stmts);
        func->returnType = $7;
        $$ = func;
        free($2);
        delete $4;
        delete $9;
    }
    ;

/* Lista de identificadores para parámetros de función */
identifier_list:
    IDENTIFIER {
        $$ = new std::vector<std::string>();
        $$->push_back($1);
        free($1);
    }
    | identifier_list COMMA IDENTIFIER {
        $$ = $1;
        $$->push_back($3);
        free($3);
    }
    ;

/* Expresiones */
expression:
    primary_expression
    /* Operadores binarios */
    | expression PLUS expression {
        $$ = new AdditionNode($1, $3);
    }
    | expression MINUS expression {
        $$ = new SubtractionNode($1, $3);
    }
    | expression MULTIPLY expression {
        $$ = new MultiplicationNode($1, $3);
    }
    | expression DIVIDE expression {
        $$ = new DivisionNode($1, $3);
    }
    | expression MODULO expression {
        $$ = new ModuloNode($1, $3);
    }
    | expression POWER expression {
        $$ = new PowerNode($1, $3);
    }
    | expression CONCAT expression {
        $$ = new ConcatenationNode($1, $3);
    }
    | expression CONCAT_SPACE expression {
        // @@ is equivalent to @ " " @
        ExpressionNode* space = new StringLiteralNode(" ");
        ExpressionNode* leftConcat = new ConcatenationNode($1, space);
        $$ = new ConcatenationNode(leftConcat, $3);
    }
    /* Operadores de comparación */
    | expression EQEQ expression {
        $$ = new EqualNode($1, $3);
    }
    | expression NOTEQ expression {
        $$ = new NotEqualNode($1, $3);
    }
    | expression LESS expression {
        $$ = new LessNode($1, $3);
    }
    | expression LESSEQ expression {
        $$ = new LessEqNode($1, $3);
    }
    | expression GREATER expression {
        $$ = new GreaterNode($1, $3);
    }
    | expression GREATEREQ expression {
        $$ = new GreaterEqNode($1, $3);
    }
    /* Operadores lógicos */
    | expression AND expression {
        $$ = new AndNode($1, $3);
    }
    | expression OR expression {
        $$ = new OrNode($1, $3);
    }
    /* Operador unario */
    | MINUS expression %prec UMINUS {
        $$ = new UnaryMinusNode($2);
    }
    | NOT expression {
        $$ = new NotNode($2);
    }
    /* Asignación */
    | IDENTIFIER ASSIGN expression {
        $$ = new AssignmentNode($1, $3);
        free($1);
    }
    /* Asignación a miembro */
    | expression DOT IDENTIFIER ASSIGN expression {
        // Check if the expression is a variable node with identifier "self"
        VariableNode* var = dynamic_cast<VariableNode*>($1);
        if (var && var->identifier == "self") {
            $$ = new SelfMemberAssignmentNode($3, $5);
            delete $1; // Delete the temporary VariableNode
        } else {
            $$ = new MemberAssignmentNode($1, $3, $5);
        }
        free($3);
    }
    /* Llamadas */
    | IDENTIFIER LPAREN expression_list_opt RPAREN {
        $$ = new FuncCallNode($1, *$3);
        free($1);
        delete $3;
    }
    /* Expresión let-in con múltiples variables */
    | LET var_decl_list IN expression {
        $$ = new LetExprNode(*$2, $4);
        delete $2;
    }
    /* Expresión if con soporte para elif */
    | IF LPAREN expression RPAREN expression elif_list ELSE expression {
        ConditionalNode* cond = new ConditionalNode();
        cond->addBranch($3, $5);
        // Agregar todas las ramas elif
        for (auto& branch : *$6) {
            cond->addBranch(branch.first, branch.second);
        }
        cond->setElse($8);
        $$ = cond;
        delete $6;
    }
    /* Expresión if sin elif */
    | IF LPAREN expression RPAREN expression ELSE expression {
        ConditionalNode* cond = new ConditionalNode();
        cond->addBranch($3, $5);
        cond->setElse($7);
        $$ = cond;
    }
    /* Expresión while */
    | WHILE LPAREN expression RPAREN expression {
        $$ = new WhileNode($3, $5);
    }
    /* Expresión for con range */
    | FOR LPAREN IDENTIFIER IN IDENTIFIER LPAREN expression COMMA expression RPAREN RPAREN expression {
        // Verificar que el identificador sea "range"
        if (std::string($5) != "range") {
            yyerror("Expected 'range' in for loop");
            $$ = nullptr;
        } else {
            $$ = new ForRangeNode($3, $7, $9, $12);
        }
        free($3);
        free($5);
    }
    /* Bloque de expresiones */
    | LBRACE expression_sequence RBRACE {
        $$ = new BlockExprNode(*$2);
        delete $2;
    }
    /* Acceso a miembro */
    | expression DOT IDENTIFIER {
        // Check if the expression is a variable node with identifier "self"
        VariableNode* var = dynamic_cast<VariableNode*>($1);
        if (var && var->identifier == "self") {
            $$ = new SelfMemberAccessNode($3);
            delete $1; // Delete the temporary VariableNode
        } else {
            $$ = new MemberAccessNode($1, $3);
        }
        free($3);
    }
    /* Llamada a método */
    | expression DOT IDENTIFIER LPAREN expression_list_opt RPAREN {
        $$ = new MethodCallNode($1, $3, *$5);
        free($3);
        delete $5;
    }
    /* Type checking with is operator */
    | expression IS IDENTIFIER {
        $$ = new IsNode($1, $3);
        free($3);
    }
    | expression IS TYPE_NUMBER {
        $$ = new IsNode($1, "Number");
    }
    | expression IS TYPE_STRING {
        $$ = new IsNode($1, "String");
    }
    | expression IS TYPE_BOOLEAN {
        $$ = new IsNode($1, "Boolean");
    }
    /* Type casting with as operator */
    | expression AS IDENTIFIER {
        $$ = new AsNode($1, $3);
        free($3);
    }
    | expression AS TYPE_NUMBER {
        $$ = new AsNode($1, "Number");
    }
    | expression AS TYPE_STRING {
        $$ = new AsNode($1, "String");
    }
    | expression AS TYPE_BOOLEAN {
        $$ = new AsNode($1, "Boolean");
    }
    ;

/* Expresiones primarias */
primary_expression:
    NUMBER {
        $$ = new NumberNode($1);
    }
    | STRING {
        $$ = new StringLiteralNode($1);
        free($1);
    }
    | TRUE {
        $$ = new BooleanNode(true);
    }
    | FALSE {
        $$ = new BooleanNode(false);
    }
    | IDENTIFIER {
        $$ = new VariableNode($1);
        free($1);
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    /* Constantes matemáticas */
    | PI {
        $$ = new NumberNode(3.14159265358979323846);
    }
    | E {
        $$ = new NumberNode(2.71828182845904523536);
    }
    /* Funciones builtin */
    | PRINT LPAREN expression RPAREN {
        $$ = new PrintExpressionNode($3);
    }
    | SQRT LPAREN expression RPAREN {
        std::vector<ExpressionNode*> args;
        args.push_back($3);
        $$ = new FuncCallNode("sqrt", args);
    }
    | SIN LPAREN expression RPAREN {
        std::vector<ExpressionNode*> args;
        args.push_back($3);
        $$ = new FuncCallNode("sin", args);
    }
    | COS LPAREN expression RPAREN {
        std::vector<ExpressionNode*> args;
        args.push_back($3);
        $$ = new FuncCallNode("cos", args);
    }
    | EXP LPAREN expression RPAREN {
        std::vector<ExpressionNode*> args;
        args.push_back($3);
        $$ = new FuncCallNode("exp", args);
    }
    | LOG LPAREN expression COMMA expression RPAREN {
        std::vector<ExpressionNode*> args;
        args.push_back($3);
        args.push_back($5);
        $$ = new FuncCallNode("log", args);
    }
    | RAND LPAREN RPAREN {
        std::vector<ExpressionNode*> args;
        $$ = new FuncCallNode("rand", args);
    }
    /* Type instantiation */
    | NEW IDENTIFIER LPAREN expression_list_opt RPAREN {
        $$ = new TypeInstantiationNode($2, *$4);
        free($2);
        delete $4;
    }
    /* Base call */
    | BASE LPAREN expression_list_opt RPAREN {
        $$ = new BaseCallNode(*$3);
        delete $3;
    }
    ;

/* Lista de expresiones (opcional) */
expression_list_opt:
    /* vacío */ {
        $$ = new std::vector<ExpressionNode*>();
    }
    | expression_list {
        $$ = $1;
    }
    ;

/* Lista de expresiones */
expression_list:
    expression {
        $$ = new std::vector<ExpressionNode*>();
        $$->push_back($1);
    }
    | expression_list COMMA expression {
        $$ = $1;
        $$->push_back($3);
    }
    ;

/* Secuencia de expresiones (para bloques y múltiples expresiones) */
expression_sequence:
    expression SEMICOLON {
        $$ = new std::vector<ExpressionNode*>();
        $$->push_back($1);
    }
    | expression_sequence expression SEMICOLON {
        $$ = $1;
        $$->push_back($2);
    }
    ;

/* Lista de declaraciones de variables para let */
var_decl_list:
    var_decl {
        $$ = new std::vector<VarDeclPair>();
        $$->push_back(*$1);
        delete $1;
    }
    | var_decl_list COMMA var_decl {
        $$ = $1;
        $$->push_back(*$3);
        delete $3;
    }
    ;

/* Declaración individual de variable */
var_decl:
    IDENTIFIER EQUALS expression {
        $$ = new VarDeclPair();
        $$->id = $1;
        $$->expr = $3;
        $$->type = nullptr;
        free($1);
    }
    | IDENTIFIER COLON type_annotation EQUALS expression {
        $$ = new VarDeclPair();
        $$->id = $1;
        $$->expr = $5;
        $$->type = $3;
        free($1);
    }
    | LET IDENTIFIER EQUALS expression {
        $$ = new VarDeclPair();
        $$->id = $2;
        $$->expr = $4;
        $$->type = nullptr;
        free($2);
    }
    | LET IDENTIFIER COLON type_annotation EQUALS expression {
        $$ = new VarDeclPair();
        $$->id = $2;
        $$->expr = $6;
        $$->type = $4;
        free($2);
    }
    ;

/* Lista de cláusulas elif */
elif_list:
    ELIF LPAREN expression RPAREN expression {
        $$ = new std::vector<std::pair<ExpressionNode*, ExpressionNode*>>();
        $$->push_back(std::make_pair($3, $5));
    }
    | elif_list ELIF LPAREN expression RPAREN expression {
        $$ = $1;
        $$->push_back(std::make_pair($4, $6));
    }
    ;

/* Lista de parámetros */
parameter_list:
    parameter {
        $$ = new std::vector<Parameter>();
        $$->push_back(*$1);
        delete $1;
    }
    | parameter_list COMMA parameter {
        $$ = $1;
        $$->push_back(*$3);
        delete $3;
    }
    ;

/* Parámetro individual */
parameter:
    IDENTIFIER {
        $$ = new Parameter($1, nullptr);
        free($1);
    }
    | IDENTIFIER COLON type_annotation {
        $$ = new Parameter($1, $3);
        free($1);
    }
    ;

%%

void yyerror(const char* s) {
    std::cerr << "Error sintáctico: " << s << " en línea " << yylineno << ", cerca de '" << yytext << "'" << std::endl;
}