#pragma once

#include <stdio.h>

#define RESET "\033[;0m"
#define BOLD "\033[0;1m"
#define BOLD_RED "\033[1;31m"

#define error(msg)                                                             \
    do {                                                                       \
        fprintf(stderr, "%scane: %serror%s: %s\n", BOLD, BOLD_RED, RESET,      \
                msg);                                                          \
    } while (0)

#define perr(msg)                                                              \
    do {                                                                       \
        fprintf(stderr, "%scane: %serror%s: ", BOLD, BOLD_RED, RESET);         \
        perror(msg);                                                           \
    } while (0)

#define MAYBE_UNUSED __attribute__((unused))
