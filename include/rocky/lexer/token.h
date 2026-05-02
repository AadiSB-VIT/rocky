#ifndef ROCKY_LEXER_TOKEN_H
#define ROCKY_LEXER_TOKEN_H

#include <stddef.h>

typedef enum {
    /*  Literals  */
    TOKEN_INT,
    TOKEN_FLOAT,

    /*  Identifiers  */
    TOKEN_IDENTIFIER,
    TOKEN_TRUE,
    TOKEN_FALSE,

    /*  Arithmetic operators  */
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,

    /*  Bitwise operators  */
    TOKEN_AMP,          /* &  */
    TOKEN_PIPE,         /* |  */
    TOKEN_CARET,        /* ^  */
    TOKEN_TILDE,        /* ~  */
    TOKEN_LSHIFT,       /* << */
    TOKEN_RSHIFT,       /* >> */

    /*  Comparison  */
    TOKEN_EQEQ,         /* == */
    TOKEN_BANGEQ,       /* != */
    TOKEN_LT,           /* <  */
    TOKEN_GT,           /* >  */
    TOKEN_LTEQ,         /* <= */
    TOKEN_GTEQ,         /* >= */

    /*  Logical  */
    TOKEN_AMPAMP,       /* && */
    TOKEN_PIPEPIPE,     /* || */
    TOKEN_BANG,         /* !  */

    /*  Assignment  */
    TOKEN_EQUALS,       /* =  */

    /*  Parentheses  */
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    /*  Special  */
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_INVALID,
} TokenKind;

/* Lexeme is represented as a slice (start pointer+length) into original source buffer */
typedef struct {
    TokenKind type;     // type of token

    const char *start;  // pointer to first character of lexeme
    size_t length;      // length of lexeme

    int line;           // line no.
    int column;         // column no.

} Token;

#endif
