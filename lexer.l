%{
#include "../include/AllHeaders.hpp"
#include "parser.hpp"

#include <cstdio>
#include <cstring>

int yylex();
extern int line_count;

%}

%option noyywrap

%%
"//".*          { /* Ignore single-line comments */ }
"/*"([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+"/" { /* Ignore multi-line comments */ }
[0-9]+  { yylval.number = atoi(yytext); return NUMBER; }
\"(\\.|[^\"\\])*\" {
    // Handle string literals with escape sequences
    char* str = strdup(yytext + 1);  // Skip opening quote
    str[strlen(str) - 1] = '\0';     // Remove closing quote

    // Process escape sequences
    char* processed = (char*)malloc(strlen(str) + 1);
    int j = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\\' && str[i + 1] != '\0') {
            switch (str[i + 1]) {
                case 'n':
                    processed[j++] = '\n';
                    i++; // Skip the next character
                    break;
                case 't':
                    processed[j++] = '\t';
                    i++; // Skip the next character
                    break;
                case '"':
                    processed[j++] = '"';
                    i++; // Skip the next character
                    break;
                case '\\':
                    processed[j++] = '\\';
                    i++; // Skip the next character
                    break;
                default:
                    // For unknown escape sequences, keep the backslash
                    processed[j++] = str[i];
                    break;
            }
        } else {
            processed[j++] = str[i];
        }
    }
    processed[j] = '\0';

    free(str);
    yylval.string = processed;
    return STRING;
}
true    { return TRUE; }      // Nuevo token para true
false   { return FALSE; }     // Nuevo token para false
print   { return PRINT; }
let     { return LET; }
in      { return IN; }
function { return FUNCTION; }
type    { return TYPE; }
inherits { return INHERITS; }
base    { return BASE; }
new     { return NEW; }
if      { return IF; }
elif    { return ELIF; }
else    { return ELSE; }
while   { return WHILE; }
for     { return FOR; }
is      { return IS; }
as      { return AS; }
=>      { return ARROW; }
Number  { return TYPE_NUMBER; }
String  { return TYPE_STRING; }
Boolean { return TYPE_BOOLEAN; }
[a-zA-Z][a-zA-Z0-9_]* {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}
\n      { line_count++; /* ignore newlines as whitespace */; }
[ \t]   { /* ignore whitespace */ }
;       { return SEMICOLON; }
":="    { return ASSIGN; }
=       { return EQUALS; }
,       { return COMMA; }
:       { return COLON; }
"("     { return LPAREN; }
")"     { return RPAREN; }
"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return MULTIPLY; }
"/"     { return DIVIDE; }
"%"     { return MODULO; }
"^"     { return POWER; }
"@@"    { return CONCAT_SPACE; }
"@"     { return CONCAT; }
"{"     { return LBRACE; }
"}"     { return RBRACE; }
"<="    { return LESSEQ; }
">="    { return GREATEREQ; }
"=="    { return EQEQ; }
"!="    { return NOTEQ; }
"<"     { return LESS; }
">"     { return GREATER; }
"&"     { return AND; }
"|"     { return OR; }
"!"     { return NOT; }
"."     { return DOT; }
.       { printf("Carácter inesperado: '%s' en línea %d\n", yytext, line_count); }
%%
