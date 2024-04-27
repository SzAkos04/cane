#pragma once

#include "stmt.h"
#include "token.h"

#define INITIAL_STMTS 4
#define GROW_FACTOR 2

#define MAX_STMTS 2048

typedef struct Parser {
    Token *tokens;
    int current;

    Stmt *(*parse)(struct Parser *);
} Parser;

Parser parser_new(Token *tokens);

void free_stmts(Stmt *stmts);
