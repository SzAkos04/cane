#include "debug.h"
#include "executor.h"
#include "literal.h"
#include "stmt.h"
#include <stdio.h>

static int execute_print(Executor *self) {
    switch (self->stmts[self->current].data.Print.expr.type) {
    case EXPR_LITERAL:
        switch (self->stmts[self->current].data.Print.expr.data.Literal.type) {
        case LITERAL_STRING:
            printf("%s\n", self->stmts[self->current]
                               .data.Print.expr.data.Literal.data.Str);
            break;
        default:
            error("not yet implemented");
            break;
        }
        break;
    default:
        error("not yet implemented");
        break;
    }
    return 0;
}

static int execute(Executor *self) {
    (void)self;

    while (self->stmts[self->current].type != STMT_EOF) {
        switch (self->stmts[self->current].type) {
        case STMT_PRINT:
            if (execute_print(self) != 0) {
                return 1;
            }
            break;
        default:
            error("not yet implemented");
            break;
        }
        self->current++;
    }

    return 0;
}

Executor executor_new(Stmt *stmts) {
    return (Executor){
        .stmts = stmts,
        .current = 0,
        .execute = execute,
    };
}
