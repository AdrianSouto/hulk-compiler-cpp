%{
/* Flex lexer for HULK language */
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "../include/AllHeaders.hpp"
#include "parser.hpp"

/* Track line and column numbers */
int yycolumn = 1;

/* Declare yylloc as extern since it's defined in the parser */
extern YYLTYPE yylloc;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

/* Helper function to handle newlines */
void handle_newline() {
    yycolumn = 1;
}

/* Helper function to duplicate strings */
char* duplicate_string(const char* str) {
    return strdup(str);
}

%}

/* Flex options */
%option noyywrap
%option yylineno

/* Regular expression definitions */
DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
NUMBER      {DIGIT}+
DECIMAL     {DIGIT}+\.{DIGIT}+
STRING      \"([^\"\n\\]|\\.)*\"
WHITESPACE  [ \t]+
NEWLINE     \n|\r\n|\r

%%

 /* Keywords */
"let"       { printf("<LET>\n"); return LET; }
"in"        { printf("<IN>\n"); return IN; }
"function"  { printf("<FUNCTION>\n"); return FUNCTION; }
"type"      { printf("<TYPE>\n"); return TYPE; }
"inherits"  { printf("<INHERITS>\n"); return INHERITS; }
"new"       { printf("<NEW>\n"); return NEW; }
"base"      { printf("<BASE>\n"); return BASE; }
"if"        { printf("<IF>\n"); return IF; }
"elif"      { printf("<ELIF>\n"); return ELIF; }
"else"      { printf("<ELSE>\n"); return ELSE; }
"while"     { printf("<WHILE>\n"); return WHILE; }
"for"       { printf("<FOR>\n"); return FOR; }
"is"        { printf("<IS>\n"); return IS; }
"as"        { printf("<AS>\n"); return AS; }
"print"     { printf("<PRINT>\n"); return PRINT; }
"true"      { printf("<TRUE>\n"); return TRUE; }
"false"     { printf("<FALSE>\n"); return FALSE; }

 /* Type keywords */
"Number"    { printf("<TYPE_NUMBER>\n"); return TYPE_NUMBER; }
"String"    { printf("<TYPE_STRING>\n"); return TYPE_STRING; }
"Boolean"   { printf("<TYPE_BOOLEAN>\n"); return TYPE_BOOLEAN; }

 /* Multi-character operators */
"=="        { printf("<EQEQ>\n"); return EQEQ; }
"!="        { printf("<NOTEQ>\n"); return NOTEQ; }
"<="        { printf("<LESSEQ>\n"); return LESSEQ; }
">="        { printf("<GREATEREQ>\n"); return GREATEREQ; }
"&&"        { printf("<AND>\n"); return AND; }
"|"        { printf("<OR>\n"); return OR; }
"@@"        { printf("<CONCAT_SPACE>\n"); return CONCAT_SPACE; }
"=>"        { printf("<ARROW>\n"); return ARROW; }
"**"        { printf("<POWER>\n"); return POWER; }
":="        { printf("<ASSIGN>\n"); return ASSIGN; }

 /* Single character operators and delimiters */
"+"         { printf("<PLUS>\n"); return PLUS; }
"-"         { printf("<MINUS>\n"); return MINUS; }
"*"         { printf("<MULTIPLY>\n"); return MULTIPLY; }
"/"         { printf("<DIVIDE>\n"); return DIVIDE; }
"%"         { printf("<MODULO>\n"); return MODULO; }
"@"         { printf("<CONCAT>\n"); return CONCAT; }
"="         { printf("<EQUALS>\n"); return EQUALS; }
"<"         { printf("<LESS>\n"); return LESS; }
">"         { printf("<GREATER>\n"); return GREATER; }
"!"         { printf("<NOT>\n"); return NOT; }
"("         { printf("<LPAREN>\n"); return LPAREN; }
")"         { printf("<RPAREN>\n"); return RPAREN; }
"{"         { printf("<LBRACE>\n"); return LBRACE; }
"}"         { printf("<RBRACE>\n"); return RBRACE; }
";"         { printf("<SEMICOLON>\n"); return SEMICOLON; }
","         { printf("<COMMA>\n"); return COMMA; }
":"         { printf("<COLON>\n"); return COLON; }
"."         { printf("<DOT>\n"); return DOT; }

 /* Identifiers */
{ID}        { 
    yylval.string = duplicate_string(yytext);
    printf("<ID:%s>\n", yytext);
    return IDENTIFIER;
}

 /* Numbers */
{DECIMAL}   {
    yylval.decimal = atof(yytext);
    printf("<NUMBER:%s>\n", yytext);
    return NUMBER;
}

{NUMBER}    {
    yylval.number = atoi(yytext);
    printf("<NUMBER:%s>\n", yytext);
    return NUMBER;
}

 /* String literals */
{STRING}    {
    /* Remove quotes and handle escape sequences */
    std::string str(yytext + 1, yyleng - 2);
    std::string result;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; i++; break;
                case 't': result += '\t'; i++; break;
                case 'r': result += '\r'; i++; break;
                case '\\': result += '\\'; i++; break;
                case '"': result += '"'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    yylval.string = duplicate_string(result.c_str());
    printf("<STRING:%s>\n", result.c_str());
    return STRING;
}

 /* Comments */
"//"[^\n]*  { /* Skip single-line comments */ }

 /* Whitespace */
{WHITESPACE} { /* Skip whitespace */ }

 /* Newlines */
{NEWLINE}   { handle_newline(); }

 /* Error handling for unmatched characters */
.           {
    printf("Error: Carácter no reconocido '%c' en línea %d, columna %d\n", 
           yytext[0], yylineno, yycolumn);
    return yytext[0];
}

%%

/* Function to set input from file */
void set_input_from_file(FILE* file) {
    yyin = file;
    yylineno = 1;
    yycolumn = 1;
}

/* Function to set input from string */
void set_input(const std::string& text) {
    yy_scan_string(text.c_str());
    yylineno = 1;
    yycolumn = 1;
}