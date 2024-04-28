#pragma once

#include "expr.h"

typedef struct Stmt {
    enum {
        STMT_EXPR,
        STMT_LET,
        STMT_PRINT,

        STMT_EOF,
        STMT_ERR = 255,
    } type;
    union {
        Expr Expression;
        struct {
            char *name;
            Expr expr;
        } Let;
        struct {
            Expr expr;
        } Print;
    } data;
} Stmt;
