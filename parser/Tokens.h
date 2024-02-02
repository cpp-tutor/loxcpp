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
        LEFT_BRACKET,
        RIGHT_BRACKET,
        COMMA,
        DOT,
        IDENTIFIER,
        STRING,
        NUMBER,
        CLASS,
        ENDCLASS,
        ELSE,
        FALSE,
        FN,
        ENDFN,
        FOR,
        ENDFOR,
        IF,
        THEN,
        ENDIF,
        NIL,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        LET,
        TO,
        WHILE,
        ENDWHILE,
        REPEAT,
        UNTIL,
        CASE,
        ENDCASE,
        WHEN,
        OF,
        EQUAL,
        ASSIGN,
        OR,
        AND,
        NOT_EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        MINUS,
        PLUS,
        SLASH,
        STAR,
        NOT,
        UMINUS,
    };

};

#endif
