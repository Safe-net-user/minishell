#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

t_val_exp	exp_dquote(t_exp *exp)
{
    if (!exp)
        return (EXP_ERROR);
    exp->index++;
    while (exp->str[exp->index]
        && exp->str[exp->index] != '"')
    {
        if (exp->str[exp->index] == '$')
        {
            if (exp_append_expansion(exp) != EXP_SUCCESS)
                return (EXP_ERROR);
        }
        else
        {
            append_ch_sb(exp->sb, exp->str[exp->index]);
            exp->index++;
        }
    }
    if (exp->str[exp->index] == '"')
        exp->index++;
    exp->state = ST_EXP_NORMAL;
    return (EXP_SUCCESS);
}
