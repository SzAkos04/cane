#include "debug.h"
#include "lexer.h"
#include "token.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Token next(Lexer *self) {
    char error_msg[64];

    while (isspace(self->buf[self->current])) {
        if (self->buf[self->current] == '\n') {
            self->line++;
        }
        self->current++;
    }

    switch (self->buf[self->current]) {
    case '(':
        self->current++;
        return (Token){
            .type = TT_LEFT_PAREN,
            .lexeme = "(",
            .line = self->line,
        };
    case ')':
        self->current++;
        return (Token){
            .type = TT_RIGHT_PAREN,
            .lexeme = ")",
            .line = self->line,
        };
    case '{':
        self->current++;
        return (Token){
            .type = TT_LEFT_BRACE,
            .lexeme = "{",
            .line = self->line,
        };
    case '}':
        self->current++;
        return (Token){
            .type = TT_RIGHT_BRACE,
            .lexeme = "}",
            .line = self->line,
        };
    case ';':
        self->current++;
        return (Token){
            .type = TT_SEMICOLON,
            .lexeme = ";",
            .line = self->line,
        };
    case ':':
        self->current++;
        return (Token){
            .type = TT_COLON,
            .lexeme = ":",
            .line = self->line,
        };
    case '!':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_BANG_EQUAL,
                .lexeme = "!=",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_BANG,
                .lexeme = "!",
                .line = self->line,
            };
        }
    case '=':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_EQUAL_EQUAL,
                .lexeme = "==",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_EQUAL,
                .lexeme = "=",
                .line = self->line,
            };
        }
    case '>':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_GREATER_EQUAL,
                .lexeme = ">=",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_GREATER,
                .lexeme = ">",
                .line = self->line,
            };
        }
    case '<':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_LESS_EQUAL,
                .lexeme = "<=",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_LESS,
                .lexeme = "<",
                .line = self->line,
            };
        }
    case '+':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_PLUS_EQUAL,
                .lexeme = "+=",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_PLUS,
                .lexeme = "+",
                .line = self->line,
            };
        }
    case '-':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_MINUS_EQUAL,
                .lexeme = "-=",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_MINUS,
                .lexeme = "-",
                .line = self->line,
            };
        }
    case '/':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_SLASH_EQUAL,
                .lexeme = "/=",
                .line = self->line,
            };
        } else if (self->buf[self->current] == '/') {
            while (self->buf[self->current] != '\n') {
                self->current++;
            }
            return (Token){.type = TT_COMMENT};
        } else if (self->buf[self->current] == '*') {
            while (self->buf[self->current - 1] == '*' &&
                   self->buf[self->current] == '/') {
                self->current++;
                if (self->buf[self->current] == '\n') {
                    self->line++;
                }
            }
            return (Token){.type = TT_COMMENT};
        } else {
            return (Token){
                .type = TT_SLASH,
                .lexeme = "/",
                .line = self->line,
            };
        }
    case '*':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_STAR_EQUAL,
                .lexeme = "*=",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_STAR,
                .lexeme = "*",
                .line = self->line,
            };
        }
    case '%':
        self->current++;
        if (self->buf[self->current] == '=') {
            self->current++;
            return (Token){
                .type = TT_MODULO_EQUAL,
                .lexeme = "%=",
                .line = self->line,
            };
        } else {
            return (Token){
                .type = TT_MODULO,
                .lexeme = "%",
                .line = self->line,
            };
        }
    case '&':
        self->current++;
        if (self->buf[self->current] == '&') {
            self->current++;
            return (Token){
                .type = TT_AND,
                .lexeme = "&&",
                .line = self->line,
            };
        } else {
            sprintf(error_msg, "unexpected token `&` at line %i", self->line);
            error(error_msg);
            return token_default();
        }
    case '|':
        self->current++;
        if (self->buf[self->current] == '|') {
            self->current++;
            return (Token){
                .type = TT_OR,
                .lexeme = "||",
                .line = self->line,
            };
        } else {
            sprintf(error_msg, "unexpected token `|` at line %i", self->line);
            error(error_msg);
            return token_default();
        }
    case '"':
        self->current++;
        int start = self->current;
        while (self->buf[self->current] != '"') {
            if ((size_t)self->current > strlen(self->buf)) {
                snprintf(error_msg, sizeof(error_msg),
                         "unterminated string at line %i", self->line);
                error(error_msg);
                return token_default();
            }
            self->current++;
        }
        char *lexeme = (char *)malloc(self->current - start + 1);
        if (!lexeme) {
            perr("failed to allocate memory");
            return token_default();
        }
        snprintf(lexeme, self->current - start + 1, "%s", self->buf + start);
        self->current++;
        return (Token){
            .type = TT_STRING,
            .lexeme = lexeme,
            .line = self->line,
        };
    case '\0':
        return (Token){
            .type = TT_EOF,
            .lexeme = NULL,
            .line = self->line,
        };
    default: {
        if (isdigit(self->buf[self->current])) {
            int start = self->current;
            while (isdigit(self->buf[self->current]) ||
                   (self->buf[self->current] == '.' &&
                    isdigit(self->buf[self->current + 1]))) {
                self->current++;
            }
            int len = self->current - start;
            char *lexeme = (char *)malloc(sizeof(char) * (len + 1));
            if (!lexeme) {
                perr("failed to allocate memory");
                return token_default();
            }
            strncpy(lexeme, self->buf + start, len);
            lexeme[len] = '\0';

            Token token = (Token){
                .type = TT_NUMBER,
                .lexeme = strdup(lexeme),
                .line = self->line,
            };
            free(lexeme);

            return token;
        } else if (isalpha(self->buf[self->current])) {
            int start = self->current;
            while (isalnum(self->buf[self->current])) {
                self->current++;
            }
            int len = self->current - start;

            char *lexeme = (char *)malloc(sizeof(char) * (len + 1));
            if (!lexeme) {
                perr("failed to allocate memory");
                return token_default();
            }
            strncpy(lexeme, self->buf + start, len);
            lexeme[len] = '\0';

            TokenType tt = TT_IDENTIFIER;
            if (!strcmp(lexeme, "let")) {
                tt = TT_LET;
            } else if (!strcmp(lexeme, "print")) {
                tt = TT_PRINT;
            } else if (!strcmp(lexeme, "true") || !strcmp(lexeme, "false")) {
                tt = TT_BOOL;
            } else if (!strcmp(lexeme, "null")) {
                tt = TT_NULL;
            }

            Token token = (Token){
                .type = tt,
                .lexeme = strdup(lexeme),
                .line = self->line,
            };
            free(lexeme);

            return token;
        }
        break;
    }
    }

    printf("%i\n", self->current);

    return token_default();
}

#define INITIAL_TOKENS 4
#define GROW_FACTOR 2

static Token *lex(Lexer *self) {
    int capacity = INITIAL_TOKENS;
    Token *tokens = malloc(sizeof(Token) * capacity);
    if (!tokens) {
        perr("failed to allocate memory");
        return NULL;
    }

    int i = 0;
    do {
        if (i >= capacity) {
            capacity *= GROW_FACTOR;
            Token *temp = realloc(tokens, sizeof(Token) * capacity);
            if (!temp) {
                perr("failed to allocate memory");
                free(tokens);
                return NULL;
            }
            tokens = temp;
        }
        tokens[i] = next(self);
        if (tokens[i].type == TT_UNKNOWN) {
            return NULL;
        }
        i++;
    } while (i < MAX_TOKENS && tokens[i].type != TT_EOF);

    Token *temp = realloc(tokens, sizeof(Token) * (i + 1));
    if (!temp) {
        perr("failed to allocate memory");
        free(tokens);
        return NULL;
    }
    tokens = temp;

    return tokens;
}

Lexer lexer_new(const char *buf) {
    return (Lexer){
        .buf = buf,
        .current = 0,
        .line = 1,

        .lex = lex,
    };
}

void free_tokens(Token *tokens) {
    for (int i = 0; tokens[i].type == TT_EOF; i++) {
        if (tokens[i].type == TT_PRINT || tokens[i].type == TT_IDENTIFIER ||
            tokens[i].type == TT_STRING || tokens[i].type == TT_NUMBER) {
            free(tokens[i].lexeme);
        }
    }
    free(tokens);
}
