#ifndef PARSER_H
#define PARSER_H

#include "rocky/ast/ast.h"
#include "rocky/arena.h"

typedef struct {
    const Token *tokens;
    int          pos;
    int          len;
    Arena       *arena;
} Parser;

void  parser_init (Parser *p, const Token *tokens, int len, Arena *arena);
Expr *parse_expr  (Parser *p, int min_bp);

#endif