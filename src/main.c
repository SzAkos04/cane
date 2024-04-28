#include "debug.h"
#include "executor.h"
#include "lexer.h"
#include "parser.h"
#include "stmt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *read_file(const char *path);

int main(int argc, char **argv) {
    if (argc < 2) {
        error("incorrect usage, for help run `camo --help`");
        return EXIT_FAILURE;
    }

    char *path = NULL;
    for (int i = 1; i < argc; i++) {
        int len = strlen(argv[i]);
        if (len > 1 && argv[i][0] == '-') {
            for (int j = 1; j < len; j++) {
                switch (argv[i][j]) {
                case 'h':
                    printf("help\n");
                    return EXIT_SUCCESS;
                case '-':
                    if (!strcmp(argv[i], "--help")) {
                        printf("help\n");
                        return EXIT_SUCCESS;
                    } else {
                        char err_msg[128];
                        snprintf(err_msg, sizeof(err_msg),
                                 "unknown argument `%s`\n"
                                 "for more info, run `cane --help`",
                                 argv[i]);
                        error(err_msg);
                        return EXIT_FAILURE;
                    }
                default: {
                    char err_msg[128];
                    snprintf(err_msg, sizeof(err_msg),
                             "unknown argument `-%c`\n"
                             "for more info, run `cane --help`",
                             argv[i][j]);
                    error(err_msg);
                    return EXIT_FAILURE;
                }
                }
            }
        } else {
            path = argv[1];

            // check if file exists
            if (access(path, F_OK | R_OK) != 0) {
                char err_msg[128];
                snprintf(err_msg, sizeof(err_msg), "`%s` file does not exist",
                         path);
                perr(err_msg);
                return EXIT_FAILURE;
            }

            // get file extension
            char *ext = strchr(path, '.');
            while (ext && ext[1] == '/') {
                ext = strchr(ext + 1, '.');
            }
            // check if the file has any extension
            if (!ext) {
                char err_msg[128];
                snprintf(err_msg, sizeof(err_msg), "`%s` file has no extension",
                         path);
                error(err_msg);
                return EXIT_FAILURE;
            }

            // check if the file has the right extension
            if (strcmp(ext, ".cn") != 0) {
                char err_msg[128];
                snprintf(err_msg, sizeof(err_msg),
                         "`%s` file extension not recognised", ext);
                error(err_msg);
                return EXIT_FAILURE;
            }
        }
    }

    // read the file into a buffer
    char *buf;
    if (!(buf = read_file(path))) {
        return EXIT_FAILURE;
    }

    Lexer lexer = lexer_new(buf);
    Token *tokens = lexer.lex(&lexer);
    // if it fails, it returns NULL, and the error message is printed
    if (!tokens) {
        free(buf);
        return EXIT_FAILURE;
    }
    free(buf);

    // continue
    Parser parser = parser_new(tokens);
    Stmt *stmts = parser.parse(&parser);
    if (!stmts) {
        free_tokens(tokens);
        return EXIT_FAILURE;
    }
    free_tokens(tokens);

    Executor executor = executor_new(stmts);
    if (executor.execute(&executor) != 0) {
        free_stmts(stmts);
        return EXIT_FAILURE;
    }
    free_stmts(stmts);

    return EXIT_SUCCESS;
}

char *read_file(const char *path) {
    FILE *fs = fopen(path, "r");
    if (!fs) {
        perr("failed to open file");
        return NULL;
    }

    // get the size of the file
    fseek(fs, 0, SEEK_END);
    long file_size = ftell(fs);
    fseek(fs, 0, SEEK_SET);

    char *buf = (char *)malloc(sizeof(char) * (file_size + 1));
    if (!buf) {
        perr("failed to allocate memory");
        fclose(fs);
        return NULL;
    }

    size_t files_read = fread(buf, sizeof(char), file_size, fs);
    if (files_read != (size_t)file_size) {
        perr("failed to read file");
        fclose(fs);
        free(buf);
        return NULL;
    }
    buf[file_size] = '\0';

    fclose(fs);

    return buf;
}
