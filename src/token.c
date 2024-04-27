#include "token.h"

#include <stddef.h>

Token token_default(void) {
    return (Token){
        .type = TT_UNKNOWN,
        .lexeme = NULL,
        .line = -1,
    };
}

bool is_unary_op(Token t) { return t.type == TT_BANG || t.type == TT_MINUS; }
