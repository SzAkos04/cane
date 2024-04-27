#pragma once

#include "stmt.h"
typedef struct Executor {
    Stmt *stmts;
    int current;

    int (*execute)(struct Executor *);
} Executor;

Executor executor_new(Stmt *stmts);
