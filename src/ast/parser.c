#include <stdlib.h>
#include <stdio.h>
#include "ast/parser.h"

/* ── Allocator ───────────────────────────────────────────── */

static Expr *alloc_expr(ExprKind kind, Token tok) {
    Expr *e  = calloc(1, sizeof(Expr));
    e->kind  = kind;
    e->token = tok;
    e->type  = TYPE_VOID;
    return e;
}

/* ── Token navigation ────────────────────────────────────── */

static Token peek(const Parser *p) {
    return p->tokens[p->pos];
}

static Token advance(Parser *p) {
    Token t = p->tokens[p->pos];
    if (t.kind != TOK_EOF) p->pos++;
    return t;
}

static Token expect(Parser *p, TokenKind kind) {
    Token t = advance(p);
    if (t.kind != kind) {
        fprintf(stderr, "parse error at line %d col %d: "
                        "expected token %d, got %d\n",
                t.line, t.col, kind, t.kind);
        exit(1);
    }
    return t;
}

/* ── Binding powers ──────────────────────────────────────── */
/*
 * Each binary operator has a left BP and a right BP.
 * Right BP > left BP  → right-associative.
 * Right BP < left BP  → left-associative  (usual case).
 * Returning 0,0 means the token is not a binary infix operator.
 *
 * Precedence ladder (low → high):
 *   ||          4
 *   &&          6
 *   == !=       8
 *   < > <= >=  10
 *   | ^         12
 *   &           14   (lower than shifts so  a & b << 1  parses as  a & (b<<1))
 *   << >>       16
 *   + -         18
 *   * / %       20
 */

typedef struct { int lbp; int rbp; } BP;

static BP infix_bp(TokenKind k) {
    switch (k) {
        case TOK_PIPEPIPE:  return (BP){ 4,  5  };
        case TOK_AMPAMP:    return (BP){ 6,  7  };
        case TOK_EQEQ:
        case TOK_BANGEQ:    return (BP){ 8,  9  };
        case TOK_LT:
        case TOK_GT:
        case TOK_LTEQ:
        case TOK_GTEQ:      return (BP){ 10, 11 };
        case TOK_PIPE:      return (BP){ 12, 13 };
        case TOK_CARET:     return (BP){ 12, 13 };
        case TOK_AMP:       return (BP){ 14, 15 };
        case TOK_LSHIFT:
        case TOK_RSHIFT:    return (BP){ 16, 17 };
        case TOK_PLUS:
        case TOK_MINUS:     return (BP){ 18, 19 };
        case TOK_STAR:
        case TOK_SLASH:
        case TOK_PERCENT:   return (BP){ 20, 21 };
        default:            return (BP){ 0,  0  };
    }
}

/* Prefix (nud) binding power — how tightly a unary op binds its operand */
static int prefix_bp(TokenKind k) {
    switch (k) {
        case TOK_MINUS:
        case TOK_TILDE:
        case TOK_BANG:  return 22;   /* tighter than any binary op */
        default:        return -1;   /* not a prefix operator      */
    }
}

/* ── Operator mapping ────────────────────────────────────── */

static UnaryOp tok_to_unop(TokenKind k) {
    switch (k) {
        case TOK_MINUS: return UNOP_NEG;
        case TOK_TILDE: return UNOP_BITNOT;
        case TOK_BANG:  return UNOP_LOGICNOT;
        default:        exit(1);   /* unreachable */
    }
}

static BinaryOp tok_to_binop(TokenKind k) {
    switch (k) {
        case TOK_PLUS:      return BINOP_ADD;
        case TOK_MINUS:     return BINOP_SUB;
        case TOK_STAR:      return BINOP_MUL;
        case TOK_SLASH:     return BINOP_DIV;
        case TOK_PERCENT:   return BINOP_MOD;
        case TOK_AMP:       return BINOP_BAND;
        case TOK_PIPE:      return BINOP_BOR;
        case TOK_CARET:     return BINOP_BXOR;
        case TOK_LSHIFT:    return BINOP_SHL;
        case TOK_RSHIFT:    return BINOP_SHR;
        case TOK_EQEQ:      return BINOP_EQ;
        case TOK_BANGEQ:    return BINOP_NEQ;
        case TOK_LT:        return BINOP_LT;
        case TOK_GT:        return BINOP_GT;
        case TOK_LTEQ:      return BINOP_LE;
        case TOK_GTEQ:      return BINOP_GE;
        case TOK_AMPAMP:    return BINOP_AND;
        case TOK_PIPEPIPE:  return BINOP_OR;
        default:            exit(1);   /* unreachable */
    }
}

/* ── Core Pratt loop ─────────────────────────────────────── */

void parser_init(Parser *p, const Token *tokens, int len) {
    p->tokens = tokens;
    p->pos    = 0;
    p->len    = len;
}

Expr *parse_expr(Parser *p, int min_bp) {
    Token  tok = advance(p);
    Expr  *lhs = NULL;

    /* ── nud: prefix / atom ──────────────────────────────── */
    switch (tok.kind) {

        case TOK_INT: {
            lhs = alloc_expr(EXPR_INT_LIT, tok);
            lhs->as.ival = tok.value.ival;
            break;
        }

        case TOK_FLOAT: {
            lhs = alloc_expr(EXPR_FLOAT_LIT, tok);
            lhs->as.fval = tok.value.fval;
            break;
        }

        case TOK_TRUE:
        case TOK_FALSE: {
            lhs = alloc_expr(EXPR_BOOL_LIT, tok);
            lhs->as.bval = (tok.kind == TOK_TRUE) ? 1 : 0;
            break;
        }

        case TOK_IDENT: {
            lhs = alloc_expr(EXPR_IDENT, tok);
            lhs->as.ident.name = tok.start;
            lhs->as.ident.len  = tok.len;
            break;
        }

        case TOK_LPAREN: {
            /* grouped expression — consume inner expr then closing ')' */
            lhs = parse_expr(p, 0);
            expect(p, TOK_RPAREN);
            break;
        }

        case TOK_MINUS:
        case TOK_TILDE:
        case TOK_BANG: {
            int   bp      = prefix_bp(tok.kind);
            Expr *operand = parse_expr(p, bp);
            lhs           = alloc_expr(EXPR_UNARY, tok);
            lhs->as.unary.op      = tok_to_unop(tok.kind);
            lhs->as.unary.operand = operand;
            break;
        }

        default: {
            fprintf(stderr, "parse error at line %d col %d: "
                            "unexpected token %d in expression\n",
                    tok.line, tok.col, tok.kind);
            exit(1);
        }
    }

    /* ── led: infix / binary ─────────────────────────────── */
    for (;;) {
        Token op  = peek(p);
        BP    bp  = infix_bp(op.kind);

        if (bp.lbp == 0 || bp.lbp <= min_bp)
            break;

        advance(p);   /* consume the operator token */

        Expr *rhs = parse_expr(p, bp.rbp);
        Expr *bin = alloc_expr(EXPR_BINARY, op);
        bin->as.binary.op  = tok_to_binop(op.kind);
        bin->as.binary.lhs = lhs;
        bin->as.binary.rhs = rhs;
        lhs = bin;
    }

    return lhs;
}

/* ── Cleanup ─────────────────────────────────────────────── */

void expr_free(Expr *e) {
    if (!e) return;
    switch (e->kind) {
        case EXPR_UNARY:
            expr_free(e->as.unary.operand);
            break;
        case EXPR_BINARY:
            expr_free(e->as.binary.lhs);
            expr_free(e->as.binary.rhs);
            break;
        case EXPR_CAST:
            expr_free(e->as.cast.operand);
            break;
        default:
            break;
    }
    free(e);
}