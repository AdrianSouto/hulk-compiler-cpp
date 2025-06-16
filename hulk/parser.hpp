/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HULK_PARSER_HPP_INCLUDED
# define YY_YY_HULK_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    STRING = 259,                  /* STRING  */
    IDENTIFIER = 260,              /* IDENTIFIER  */
    PRINT = 261,                   /* PRINT  */
    EOL = 262,                     /* EOL  */
    LET = 263,                     /* LET  */
    IN = 264,                      /* IN  */
    FUNCTION = 265,                /* FUNCTION  */
    ARROW = 266,                   /* ARROW  */
    TYPE = 267,                    /* TYPE  */
    INHERITS = 268,                /* INHERITS  */
    BASE = 269,                    /* BASE  */
    NEW = 270,                     /* NEW  */
    IF = 271,                      /* IF  */
    ELIF = 272,                    /* ELIF  */
    ELSE = 273,                    /* ELSE  */
    IS = 274,                      /* IS  */
    AS = 275,                      /* AS  */
    LPAREN = 276,                  /* LPAREN  */
    RPAREN = 277,                  /* RPAREN  */
    LBRACE = 278,                  /* LBRACE  */
    RBRACE = 279,                  /* RBRACE  */
    TYPE_NUMBER = 280,             /* TYPE_NUMBER  */
    TYPE_STRING = 281,             /* TYPE_STRING  */
    TYPE_BOOLEAN = 282,            /* TYPE_BOOLEAN  */
    TRUE = 283,                    /* TRUE  */
    FALSE = 284,                   /* FALSE  */
    WHILE = 285,                   /* WHILE  */
    FOR = 286,                     /* FOR  */
    PLUS = 287,                    /* PLUS  */
    MINUS = 288,                   /* MINUS  */
    MULTIPLY = 289,                /* MULTIPLY  */
    DIVIDE = 290,                  /* DIVIDE  */
    MODULO = 291,                  /* MODULO  */
    COLON = 292,                   /* COLON  */
    POWER = 293,                   /* POWER  */
    CONCAT = 294,                  /* CONCAT  */
    CONCAT_SPACE = 295,            /* CONCAT_SPACE  */
    SEMICOLON = 296,               /* SEMICOLON  */
    COMMA = 297,                   /* COMMA  */
    EQUALS = 298,                  /* EQUALS  */
    EQEQ = 299,                    /* EQEQ  */
    AND = 300,                     /* AND  */
    GREATEREQ = 301,               /* GREATEREQ  */
    NOTEQ = 302,                   /* NOTEQ  */
    LESS = 303,                    /* LESS  */
    GREATER = 304,                 /* GREATER  */
    OR = 305,                      /* OR  */
    NOT = 306,                     /* NOT  */
    LESSEQ = 307,                  /* LESSEQ  */
    ASSIGN = 308,                  /* ASSIGN  */
    DOT = 309                      /* DOT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 30 "parser.y"

    int number;
    double decimal;
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

#line 135 "hulk/parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_HULK_PARSER_HPP_INCLUDED  */
