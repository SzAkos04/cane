#include "token.h"

#include <stddef.h>

Token token_default(void) {
    return (Token){
        .type = TT_UNKNOWN,
        .lexeme = NULL,
        .line = -1,
    };
}
