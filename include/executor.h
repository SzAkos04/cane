#pragma once

#include "literal.h"
#include "stmt.h"

typedef struct {
    char *name;
    Literal value;
} Var;

#define MAX_VARS 256

typedef struct Executor {
    Stmt *stmts;
    int current;

    Var *vars;
    int varc;

    int (*execute)(struct Executor *);
} Executor;

Executor executor_new(Stmt *stmts);
