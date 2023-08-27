#ifndef INCLUDED_TOKENS_
#define INCLUDED_TOKENS_

struct Tokens
{
    // Symbolic tokens:
    enum Tokens_
    {
        LEFT_PAREN = 257,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        SEMICOLON,
        IDENTIFIER,
        STRING,
        NUMBER,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,
        EQUAL,
        OR,
        AND,
        BANG_EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        MINUS,
        PLUS,
        SLASH,
        STAR,
        BANG,
        UMINUS,
    };

};

#endif
