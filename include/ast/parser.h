#ifndef PARSER_H
#define PARSER_H

#include "ast/ast.h"
#include "ast/arena.h"
#include "ast/token.h"

typedef struct {
    const Token *tokens;   /* flat token array from lexer  */
    int          pos;      /* index of current token       */
    int          len;      /* total number of tokens       */
    Arena       *arena;
} Parser;

void  parser_init  (Parser *p, const Token *tokens, int len, Arena *arena);
Expr *parse_expr   (Parser *p, int min_bp);
void  expr_free    (Expr *e);

#endif /* PARSER_H */