#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>


enum TokenType {
    
    NUMBER,
    STRING,
    IDENTIFIER,
    TRUE,
    FALSE,
    
    
    PRINT,
    LET,
    IN,
    FUNCTION,
    TYPE,
    INHERITS,
    BASE,
    NEW,
    IF,
    ELIF,
    ELSE,
    WHILE,
    FOR,
    IS,
    AS,
    
    
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOLEAN,
    
    
    PLUS,           
    MINUS,          
    MULTIPLY,       
    DIVIDE,         
    MODULO,         
    POWER,          
    CONCAT,         
    CONCAT_SPACE,   
    
    
    EQUALS,         
    EQEQ,           
    NOTEQ,          
    LESS,           
    GREATER,        
    LESSEQ,         
    GREATEREQ,      
    
    
    AND,            
    OR,             
    NOT,            
    
    
    ASSIGN,         
    
    
    SEMICOLON,      
    COMMA,          
    COLON,          
    DOT,            
    ARROW,          
    
    
    LPAREN,         
    RPAREN,         
    LBRACE,         
    RBRACE,         
    
    
    EOL,            
    EOF_TOKEN,      
    UNKNOWN         
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    
    int number_value;
    
    Token(TokenType t = UNKNOWN, const std::string& v = "", int l = 1, int c = 1) 
        : type(t), value(v), line(l), column(c), number_value(0) {}
};


std::string tokenTypeToString(TokenType type);

#endif 