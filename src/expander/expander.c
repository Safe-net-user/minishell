#include "minishell.h"
#include "expander.h"
#include "parser.h"
#include "ft_hashtable.h"
#include <stdlib.h>

static void	init_lut(t_exp_variant_fn *lut)
{
	lut[ST_EXP_NORMAL] = exp_normal;
	lut[ST_EXP_SQUOTE] = exp_squote;
	lut[ST_EXP_DQUOTE] = exp_dquote;
}

t_exp	*init_expander(t_mms *mms)
{
	t_exp	*exp;

	exp = malloc(sizeof(*exp));
	if (!exp)
		return (NULL);
	exp->index = 0;
	exp->str = NULL;
	exp->state = ST_EXP_NORMAL;
	exp->mms = mms;
	exp->sb = init_sb(DEFAULT_SB_SIZE);
	if (!exp->sb)
	{
		free(exp);
		return (NULL);
	}
	return (exp);
}

static void	reset_expander(t_exp *exp, char *str)
{
	exp->str = str;
	exp->index = 0;
	exp->state = ST_EXP_NORMAL;
	clear_sb(exp->sb);
}

int	exp_squote(t_exp *exp)
{
	if (exp->str[exp->index] == '$')
		exp->index += 2;
	else
		exp->index++;
	while (exp->str[exp->index]
		&& exp->str[exp->index] != '\'')
	{
		append_ch_sb(exp->sb, exp->str[exp->index]);
		exp->index++;
	}
	if (exp->str[exp->index] == '\'')
		exp->index++;
	exp->state = ST_EXP_NORMAL;
	return (1);
}

static int	exp_append_expansion(t_exp *exp)
{
	char	buffer[BUFFER_TOKEN];
	char	*value;
	int		i;

	i = 0;
	exp->index++;
	while (is_valid_expansion_ch(exp->str[exp->index]))
	{
		if (i >= BUFFER_TOKEN - 1)
			return (0);
		buffer[i++] = exp->str[exp->index++];
	}
	buffer[i] = '\0';
	value = get_pointer(exp->mms->env, buffer);
	if (value)
		append_sb(exp->sb, value);
	return (1);
}

int	exp_dquote(t_exp *exp)
{
	exp->index++;
	while (exp->str[exp->index]
		&& exp->str[exp->index] != '"')
	{
		if (exp->str[exp->index] == '$')
			exp_append_expansion(exp);
		else
		{
			append_ch_sb(exp->sb, exp->str[exp->index]);
			exp->index++;
		}
	}
	if (exp->str[exp->index] == '"')
		exp->index++;
	exp->state = ST_EXP_NORMAL;
	return (1);
}

int	exp_normal(t_exp *exp)
{
	char	c;

	c = exp->str[exp->index];
	if (c == '\'' || (c == '$'
			&& exp->str[exp->index + 1] == '\''))
		exp->state = ST_EXP_SQUOTE;
	else if (c == '"')
		exp->state = ST_EXP_DQUOTE;
	else if (c == '$')
		return (exp_append_expansion(exp));
	else
	{
		append_ch_sb(exp->sb, c);
		exp->index++;
	}
	return (1);
}

static int	expand_word(t_exp *exp, t_exp_variant_fn *lut)
{
	while (exp->str[exp->index])
	{
		if (!lut[exp->state](exp))
			return (0);
	}
	return (1);
}

int	expand(t_mms *mms, t_tk ***tks)
{
	t_exp				*exp;
	t_exp_variant_fn	lut[ST_EXP_MAX];
	size_t				i;

	if (!mms || !tks || !*tks)
		return (0);
	exp = init_expander(mms);
	if (!exp)
		return (0);
	init_lut(lut);
	i = 0;
	while ((*tks)[i])
	{
		reset_expander(exp, (*tks)[i]->value);
		if (is_valid_type((tks)[i]->type))
		{
			if (!expand_word(exp, lut))
				return (0);
			free((*tks)[i]->value);
			(*tks)[i]->value = ft_strdup(exp->sb->str);
			i++;
		}
	}
	free_sb(exp->sb);
	free(exp);
	return (1);
}