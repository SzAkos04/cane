#include "debug.h"
#include "executor.h"
#include "expr.h"
#include "literal.h"
#include "stmt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int execute_let(Executor *self) {
    char err_msg[128];
    // check if variable is already declared
    for (int i = 0; i < self->varc; i++) {
        if (!strcmp(self->stmts[self->current].data.Let.name,
                    self->vars[i].name)) {
            snprintf(err_msg, sizeof(err_msg),
                     "variable `%s` is already declared",
                     self->stmts[self->current].data.Let.name);
            error(err_msg);
            return 1;
        }
    }

    switch (self->stmts[self->current].data.Let.expr.type) {
    case EXPR_UNARY:
        error("unary expressions not yet implemented");
        return 1;
    case EXPR_BINARY:
        error("binary expressions not yet implemented");
        return 1;
    case EXPR_LITERAL:
        self->vars[self->varc++] = (Var){
            .name = self->stmts[self->current].data.Let.name,
            .value = self->stmts[self->current].data.Let.expr.data.Literal,
        };
        return 0;
    case EXPR_VARIABLE:
        for (int i = 0; i < MAX_VARS; i++) {
            if (!strcmp(self->stmts[self->current].data.Let.expr.data.Variable,
                        self->vars[i].name)) {
                self->vars[self->varc++] = (Var){
                    .name = self->stmts[self->current].data.Let.name,
                    .value = self->vars[i].value,
                };
                return 0;
            }
        }
        snprintf(err_msg, sizeof(err_msg), "use of undeclared variable %s",
                 self->stmts[self->current].data.Let.expr.data.Variable);
        error(err_msg);
        return 1;
    case EXPR_ASSIGNMENT:
        error("assignment expressions not yet supported");
        return 1;
    default:
        error("not an expression");
        return 1;
    }
}

static int print_literal(Literal literal) {
    switch (literal.type) {
    case LITERAL_STRING:
        printf("%s\n", literal.data.Str);
        break;
    case LITERAL_INTEGER:
        printf("%ld\n", literal.data.Integer);
        break;
    case LITERAL_FLOAT:
        printf("%g\n", literal.data.Float);
        break;
    case LITERAL_BOOLEAN:
        printf("%s\n", literal.data.Boolean ? "true" : "false");
        break;
    case LITERAL_NULL:
        printf("(null)\n");
        break;
    default:
        error("not yet implemented");
        return 1;
    }
    return 0;
}

static int execute_print(Executor *self) {
    char err_msg[128];
    switch (self->stmts[self->current].data.Print.expr.type) {
    case EXPR_UNARY:
        error("unary expressions not yet implemented");
        return 1;
    case EXPR_BINARY:
        error("binary expressions not yet implemented");
        return 1;
    case EXPR_LITERAL:
        return print_literal(
            self->stmts[self->current].data.Print.expr.data.Literal);
    case EXPR_VARIABLE:
        for (int i = 0; i < MAX_VARS; i++) {
            if (!strcmp(
                    self->stmts[self->current].data.Print.expr.data.Variable,
                    self->vars[i].name)) {
                return print_literal(self->vars[i].value);
            }
        }
        snprintf(err_msg, sizeof(err_msg), "use of undeclared variable %s",
                 self->stmts[self->current].data.Let.expr.data.Variable);
        error(err_msg);
        return 1;
        break;
    default:
        error("not an expression");
        return 1;
    }
    return 0;
}

static int execute(Executor *self) {
    while (self->stmts[self->current].type != STMT_EOF) {
        switch (self->stmts[self->current].type) {
        case STMT_LET:
            if (execute_let(self) != 0) {
                return 1;
            }
            break;
        case STMT_PRINT:
            if (execute_print(self) != 0) {
                return 1;
            }
            break;
        default:
            error("ohter statements not yet implemented (executor)");
            return 1;
        }
        self->current++;
    }

    return 0;
}

Executor executor_new(Stmt *stmts) {
    Executor e;
    e.stmts = stmts;
    e.current = 0, e.vars = (Var *)malloc(sizeof(Var) * MAX_VARS);
    if (!e.vars) {
        perr("failed to allocate memory");
        return (Executor){0};
    }
    e.varc = 0;
    e.execute = execute;

    return e;
}

void free_executor(Executor e) { free(e.vars); }
