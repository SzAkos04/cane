#include "debug.h"
#include "expr.h"
#include "literal.h"
#include "parser.h"
#include "stmt.h"
#include "token.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Literal parse_literal(Parser *self) {
    Token token = self->tokens[self->current];

    switch (token.type) {
    case TT_IDENTIFIER:
        return (Literal){
            .type = LITERAL_IDENTIFIER,
            .data.Identifier = self->tokens[self->current].lexeme,
        };
    case TT_STRING:
        return (Literal){
            .type = LITERAL_STRING,
            .data.Str = self->tokens[self->current].lexeme,
        };
    case TT_NUMBER:
        if (floor(strtod(token.lexeme, NULL)) == strtod(token.lexeme, NULL)) {
            return (Literal){
                .type = LITERAL_INTEGER,
                .data.Integer = strtol(token.lexeme, NULL, 10),
            };
        } else {
            return (Literal){
                .type = LITERAL_FLOAT,
                .data.Float = strtod(token.lexeme, NULL),
            };
        }
    case TT_BOOL:
        return (Literal){
            .type = LITERAL_BOOLEAN,
            .data.Boolean = !strcmp(token.lexeme, "true"),
        };
    case TT_NULL:
        return (Literal){
            .type = LITERAL_NULL,
            .data.null = NULL,
        };
    default:
        error("not a literal");
        return (Literal){0};
    }
}

static Expr parse_expr(Parser *self);

static Expr parse_unary(Parser *self) {
    Token op = self->tokens[self->current];
    self->current++; // consume operator
    Expr right = parse_expr(self);
    return (Expr){
        .type = EXPR_UNARY,
        .data.Unary =
            {
                .operator= op,
                .right = &right,
            },
    };
}

static Expr parse_expr(Parser *self) {
    // TODO: temporary
    if (is_unary_op(self->tokens[self->current])) {
        return parse_unary(self);
    } else if (self->tokens[self->current].type == TT_STRING ||
               self->tokens[self->current].type == TT_NUMBER ||
               self->tokens[self->current].type == TT_BOOL ||
               self->tokens[self->current].type == TT_NULL) {
        return (Expr){
            .type = EXPR_LITERAL,
            .data.Literal = parse_literal(self),
        };
    }

    return (Expr){
        .type = EXPR_VARIABLE,
        .data.Variable = self->tokens[self->current].lexeme,
    };
}

static Stmt parse_let(Parser *self) {
    char err_msg[128];
    if (self->tokens[self->current].type != TT_LET) {
        snprintf(err_msg, sizeof(err_msg),
                 "expected `let` keyword at line %d, found %s",
                 self->tokens[self->current].line,
                 self->tokens[self->current].lexeme);
        error(err_msg);
        return (Stmt){.type = STMT_ERR};
    }
    self->current++; // consume `let`

    if (self->tokens[self->current].type != TT_IDENTIFIER) {
        snprintf(err_msg, sizeof(err_msg),
                 "expected identifier at line %d, found %s",
                 self->tokens[self->current].line,
                 self->tokens[self->current].lexeme);
        error(err_msg);
        return (Stmt){.type = STMT_ERR};
    }
    char *name = self->tokens[self->current].lexeme;
    self->current++; // consume identifiers

    if (self->tokens[self->current].type == TT_SEMICOLON) {
        self->current++; // consume `;`
        return (Stmt){
            .type = STMT_LET,
            .data.Let =
                {
                    .name = name,
                    .expr = (Expr){0},
                },
        };
    } else if (self->tokens[self->current].type == TT_EQUAL) {
        self->current++; // consume `=`
        Expr expr = parse_expr(self);
        self->current++; // consume expression's last element
        if (self->tokens[self->current].type != TT_SEMICOLON) {
            snprintf(err_msg, sizeof(err_msg),
                     "expected `;` at line %d, found %s",
                     self->tokens[self->current].line,
                     self->tokens[self->current].lexeme);
            error(err_msg);
            return (Stmt){.type = STMT_ERR};
        }
        self->current++; // consume `;`
        return (Stmt){
            .type = STMT_LET,
            .data.Let =
                {
                    .name = name,
                    .expr = expr,
                },
        };
    }

    snprintf(
        err_msg, sizeof(err_msg), "expected `;` or `=` at line %d, found %s",
        self->tokens[self->current].line, self->tokens[self->current].lexeme);
    error(err_msg);
    return (Stmt){.type = STMT_ERR};
}

static Stmt parse_print(Parser *self) {
    char err_msg[128];
    if (self->tokens[self->current].type != TT_PRINT) {
        snprintf(err_msg, sizeof(err_msg),
                 "expected `print` keyword at line %d, found %s",
                 self->tokens[self->current].line,
                 self->tokens[self->current].lexeme);
        error(err_msg);
        return (Stmt){.type = STMT_ERR};
    }
    self->current++; // consume `print`

    if (self->tokens[self->current].type != TT_LEFT_PAREN) {
        snprintf(err_msg, sizeof(err_msg), "expected `(` at line %d, found %s",
                 self->tokens[self->current].line,
                 self->tokens[self->current].lexeme);
        error(err_msg);
        return (Stmt){.type = STMT_ERR};
    }
    self->current++; // consume `(`

    Expr expr = parse_expr(self);
    self->current++; // consume the expression's last element

    if (self->tokens[self->current].type != TT_RIGHT_PAREN) {
        snprintf(err_msg, sizeof(err_msg), "expected `)` at line %d, found %s",
                 self->tokens[self->current].line,
                 self->tokens[self->current].lexeme);
        error(err_msg);
        return (Stmt){.type = STMT_ERR};
    }
    self->current++; // consume `)`

    if (self->tokens[self->current].type != TT_SEMICOLON) {
        snprintf(err_msg, sizeof(err_msg), "expected `;` at line %d, found %s",
                 self->tokens[self->current].line,
                 self->tokens[self->current].lexeme);
        error(err_msg);
        return (Stmt){.type = STMT_ERR};
    }
    self->current++; // consume `;`

    return (Stmt){
        .type = STMT_PRINT,
        .data =
            {
                .Print.expr = expr,
            },
    };
}

static Stmt parse_stmt(Parser *self) {
    switch (self->tokens[self->current].type) {
    case TT_LET:
        return parse_let(self);
    case TT_PRINT:
        return parse_print(self);
    case TT_EOF:
        return (Stmt){.type = STMT_EOF};
    default:
        /* printf("%d\n", self->tokens[self->current].type); */
        printf("%s\n", self->tokens[self->current].lexeme);
        error("other statements not yet implemented (parser)");
        return (Stmt){.type = STMT_ERR};
    }
}

static Stmt *parse(Parser *self) {
    int capacity = INITIAL_STMTS;
    Stmt *stmts = malloc(sizeof(Stmt) * capacity);
    if (!stmts) {
        perr("failed to allocate memory");
        return NULL;
    }

    int i = 0;
    do {
        if (i >= capacity) {
            capacity *= GROW_FACTOR;
            Stmt *temp = realloc(stmts, sizeof(Stmt) * capacity);
            if (!temp) {
                perr("failed to allocate memory");
                free(stmts);
                return NULL;
            }
            stmts = temp;
        }
        stmts[i] = parse_stmt(self);
        if (stmts[i].type == STMT_ERR) {
            free(stmts);
            return NULL;
        }
        i++;
    } while (i < MAX_STMTS && stmts[i - 1].type != STMT_EOF);

    Stmt *temp = realloc(stmts, sizeof(Stmt) * (i + 1));
    if (!temp) {
        perr("failed to allocate memory");
        free(stmts);
        return NULL;
    }
    stmts = temp;

    return stmts;
}

Parser parser_new(Token *tokens) {
    return (Parser){
        .tokens = tokens,
        .current = 0,

        .parse = parse,
    };
}

// TODO: Make this better
void free_stmts(Stmt *stmts) { free(stmts); }
