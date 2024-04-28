#pragma once

#include <stdbool.h>

typedef enum {
    // single character tokens
    TT_LEFT_PAREN,  // `(`
    TT_RIGHT_PAREN, // `)`
    TT_LEFT_BRACE,  // `{`
    TT_RIGHT_BRACE, // `}`
    TT_SEMICOLON,   // `;`
    TT_COLON,       // `:`

    // one or two character tokens
    TT_BANG,          // `!`
    TT_BANG_EQUAL,    // `!=`
    TT_EQUAL,         // `=`
    TT_EQUAL_EQUAL,   // `==`
    TT_GREATER,       // `>`
    TT_GREATER_EQUAL, // `>=`
    TT_LESS,          // `<`
    TT_LESS_EQUAL,    // `<=`
    TT_PLUS,          // `+`
    TT_PLUS_EQUAL,    // `+=`
    TT_MINUS,         // `-`
    TT_MINUS_EQUAL,   // `-=`
    TT_SLASH,         // `/`
    TT_SLASH_EQUAL,   // `/=`
    TT_STAR,          // `*`
    TT_STAR_EQUAL,    // `*=`
    TT_MODULO,        // `%`
    TT_MODULO_EQUAL,  // `%=`

    // two character tokens
    TT_AND, // `&&`
    TT_OR,  // `||`

    // literals
    TT_IDENTIFIER,
    TT_STRING,
    TT_NUMBER,
    TT_BOOL,
    TT_NULL,

    // keywords
    TT_PRINT, // `print`

    TT_COMMENT,

    TT_EOF,
    TT_UNKNOWN,
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
    int line;
} Token;

Token token_default(void);

bool is_unary_op(Token);
