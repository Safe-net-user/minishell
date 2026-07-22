#ifndef EXPANDER_H
#define EXPANDER_H

# include "minishell.h"
# include <stddef.h>

typedef enum e_state_exp
{
    ST_EXP_NORMAL,
    ST_EXP_SQUOTE,
    ST_EXP_DQUOTE,
}   t_state_exp;

typedef struct s_exp
{
    char        *str;
    t_sb        *sb;
    t_mms       *mms;
    size_t      index;
    t_state_exp state;
}   t_exp;

typedef int (*t_exp_variant_fn)(t_exp *);
#endif
