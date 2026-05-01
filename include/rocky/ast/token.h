#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

typedef enum {
    /* Literals & identifiers */
    TOK_INT,        /* 42, 0xFF, 0b1010          */
    TOK_FLOAT,      /* 3.14, 1.0e-9              */
    TOK_IDENT,      /* variable / function name  */
    TOK_TRUE,       /* true                      */
    TOK_FALSE,      /* false                     */

    /* Arithmetic operators */
    TOK_PLUS,       /* +  */
    TOK_MINUS,      /* -  */
    TOK_STAR,       /* *  */
    TOK_SLASH,      /* /  */
    TOK_PERCENT,    /* %  */

    /* Bitwise operators */
    TOK_AMP,        /* &  */
    TOK_PIPE,       /* |  */
    TOK_CARET,      /* ^  */
    TOK_TILDE,      /* ~  (unary only) */
    TOK_LSHIFT,     /* << */
    TOK_RSHIFT,     /* >> */

    /* Comparison & logical */
    TOK_EQEQ,       /* == */
    TOK_BANGEQ,     /* != */
    TOK_LT,         /* <  */
    TOK_GT,         /* >  */
    TOK_LTEQ,       /* <= */
    TOK_GTEQ,       /* >= */
    TOK_AMPAMP,     /* && */
    TOK_PIPEPIPE,   /* || */
    TOK_BANG,       /* !  (unary only) */

    /* Structure */
    TOK_LPAREN,     /* (  */
    TOK_RPAREN,     /* )  */
    TOK_SEMICOLON,  /* ;  */
    TOK_EOF,
} TokenKind;

typedef struct {
    TokenKind kind;
    const char *start;  /* pointer into source buffer  */
    int         len;    /* byte length of lexeme        */
    int         line;
    int         col;
    union {
        int64_t  ival;  /* TOK_INT   */
        double   fval;  /* TOK_FLOAT */
    } value;
} Token;

#endif /* TOKEN_H */