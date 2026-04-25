#ifndef ROCKY_LEXER_TOKEN_H
#define ROCKY_LEXER_TOKEN_H

#include <stddef.h>

/*
 * TokenType:
 * Represents all possible token kinds.
 * Can add new tokens here.
 */
typedef enum {
    /*  Literals  */
    TOKEN_INT,
    TOKEN_FLOAT,

    /*  Identifiers  */
    TOKEN_IDENTIFIER,

    /*  Operators  */
    TOKEN_PLUS,      
    TOKEN_MINUS,     
    TOKEN_STAR,      
    TOKEN_SLASH,     
    TOKEN_PERCENT,   
    TOKEN_EQUALS,    

    /*  Parentheses  */
    TOKEN_LPAREN,    
    TOKEN_RPAREN,    

    /*  Special  */
    TOKEN_EOF,
    TOKEN_INVALID

} TokenType;

/*
 * Token:
 * Represents a lexeme in the source code.
 * Uses pointer + length
 */
typedef struct {
    TokenType type;

    const char *start;  // points into original source
    size_t length;      // length of lexeme

    int line;           // line number (1-based)
    int column;         // column number (start position)

} Token;

#endif /* ROCKY_LEXER_TOKEN_H */
