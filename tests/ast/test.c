#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rocky/ast/parser.h"

/* ── pretty-printer ──────────────────────────────────────── */

static void print_expr(const Expr *e, int depth) {
    for (int i = 0; i < depth; i++) printf("  ");

    switch (e->kind) {
        case EXPR_INT_LIT:
            printf("INT(%lld)\n", (long long)e->as.ival);
            break;
        case EXPR_FLOAT_LIT:
            printf("FLOAT(%g)\n", e->as.fval);
            break;
        case EXPR_BOOL_LIT:
            printf("BOOL(%s)\n", e->as.bval ? "true" : "false");
            break;
        case EXPR_IDENT:
            printf("IDENT(%.*s)\n", e->as.ident.len, e->as.ident.name);
            break;
        case EXPR_UNARY: {
            const char *op = "?";
            switch (e->as.unary.op) {
                case UNOP_NEG:      op = "-";  break;
                case UNOP_BITNOT:   op = "~";  break;
                case UNOP_LOGICNOT: op = "!";  break;
            }
            printf("UNARY(%s)\n", op);
            print_expr(e->as.unary.operand, depth + 1);
            break;
        }
        case EXPR_BINARY: {
            const char *op = "?";
            switch (e->as.binary.op) {
                case BINOP_ADD:  op = "+";   break;
                case BINOP_SUB:  op = "-";   break;
                case BINOP_MUL:  op = "*";   break;
                case BINOP_DIV:  op = "/";   break;
                case BINOP_MOD:  op = "%";   break;
                case BINOP_BAND: op = "&";   break;
                case BINOP_BOR:  op = "|";   break;
                case BINOP_BXOR: op = "^";   break;
                case BINOP_SHL:  op = "<<";  break;
                case BINOP_SHR:  op = ">>";  break;
                case BINOP_EQ:   op = "==";  break;
                case BINOP_NEQ:  op = "!=";  break;
                case BINOP_LT:   op = "<";   break;
                case BINOP_GT:   op = ">";   break;
                case BINOP_LE:   op = "<=";  break;
                case BINOP_GE:   op = ">=";  break;
                case BINOP_AND:  op = "&&";  break;
                case BINOP_OR:   op = "||";  break;
            }
            printf("BINARY(%s)\n", op);
            print_expr(e->as.binary.lhs, depth + 1);
            print_expr(e->as.binary.rhs, depth + 1);
            break;
        }
        case EXPR_CAST:
            printf("CAST\n");
            print_expr(e->as.cast.operand, depth + 1);
            break;
    }
}

/* ── token builder helpers ───────────────────────────────── */

static Token tok_int(long long v) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "%lld", v);
    Token t = {0};
    t.type   = TOKEN_INT;
    t.start  = buf;
    t.length = strlen(buf);
    t.line = 1; t.column = 1;
    return t;
}

static Token tok_float(double v) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "%g", v);
    Token t = {0};
    t.type   = TOKEN_FLOAT;
    t.start  = buf;
    t.length = strlen(buf);
    t.line = 1; t.column = 1;
    return t;
}

static Token tok_op(TokenType type) {
    Token t = {0};
    t.type   = type;
    t.line   = 1; t.column = 1;
    return t;
}

static Token tok_ident(const char *name, size_t len) {
    Token t = {0};
    t.type   = TOKEN_IDENTIFIER;
    t.start  = name;
    t.length = len;
    t.line   = 1; t.column = 1;
    return t;
}

static Token tok_eof(void) {
    Token t = {0};
    t.type = TOKEN_EOF;
    return t;
}

/* ── run one test ────────────────────────────────────────── */

static void run(const char *label, Token *tokens, int len, Arena *arena) {
    printf("=== %s ===\n", label);
    Parser p;
    parser_init(&p, tokens, len, arena);
    Expr *tree = parse_expr(&p, 0);
    print_expr(tree, 0);
    arena->used = 0;
    printf("\n");
}

/* ── tests ───────────────────────────────────────────────── */

int main(void) {
    Arena arena;
    arena_init(&arena, 4096);

    {
        Token toks[] = {
            tok_int(1), tok_op(TOKEN_PLUS),
            tok_int(2), tok_op(TOKEN_STAR),
            tok_int(3), tok_eof()
        };
        run("1 + 2 * 3", toks, 6, &arena);
    }

    {
        static const char name[] = "x";
        Token toks[] = {
            tok_op(TOKEN_MINUS), tok_op(TOKEN_TILDE),
            tok_ident(name, 1), tok_eof()
        };
        run("-~x", toks, 4, &arena);
    }

    {
        static const char a[] = "a", b[] = "b";
        Token toks[] = {
            tok_ident(a, 1), tok_op(TOKEN_AMP),
            tok_ident(b, 1), tok_op(TOKEN_LSHIFT),
            tok_int(1),      tok_eof()
        };
        run("a & b << 1", toks, 6, &arena);
    }

    {
        Token toks[] = {
            tok_op(TOKEN_LPAREN),
            tok_int(1), tok_op(TOKEN_PLUS), tok_int(2),
            tok_op(TOKEN_RPAREN),
            tok_op(TOKEN_STAR), tok_int(3), tok_eof()
        };
        run("(1 + 2) * 3", toks, 8, &arena);
    }

    {
        Token toks[] = {
            tok_float(1.5), tok_op(TOKEN_PLUS),
            tok_float(2.5), tok_eof()
        };
        run("1.5 + 2.5", toks, 4, &arena);
    }

    {
        static const char a[] = "a", b[] = "b", c[] = "c";
        Token toks[] = {
            tok_ident(a, 1), tok_op(TOKEN_PIPEPIPE),
            tok_ident(b, 1), tok_op(TOKEN_AMPAMP),
            tok_ident(c, 1), tok_eof()
        };
        run("a || b && c", toks, 6, &arena);
    }

    arena_free(&arena);
    return 0;
}