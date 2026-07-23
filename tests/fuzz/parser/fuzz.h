#ifndef FUZZ_H
# define FUZZ_H

# include "../../../include/minishell.h"
# include "../../../include/parser.h"

# include <stdlib.h>
# include <stdio.h>
# include <time.h>
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

#define MAX_TOKENS 32
#define OPS_COUNT 7
#define WORDS_COUNT 8

static const char *words[] = {
    "ls", "cat", "echo", "grep",
    "foo", "bar", "in", "out",
    NULL
};

static t_type_tk ops[] = {
    TOK_PIPE,
    TOK_AND_IF,
    TOK_OR_IF,
    TOK_LESS,
    TOK_GREAT,
    TOK_DLESS,
    TOK_DGREAT
};

t_tk *generate_tokens(t_mms *mms);

#endif