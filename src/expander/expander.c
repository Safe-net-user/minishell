#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

t_exp	*init_expander(t_mms *mms)
{
	t_exp	*exp;

	if (!mms)
		return (NULL);
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
	if (!exp || !str || !exp->sb)
		return ;
	exp->str = str;
	exp->index = 0;
	exp->state = ST_EXP_NORMAL;
	clear_sb(exp->sb);
}

t_val_exp	exp_squote(t_exp *exp)
{
	if (!exp)
		return (EXP_ERROR);
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
	return (EXP_SUCCESS);
}

static t_val_exp	exp_append_expansion(t_exp *exp)
{
	char		buffer[ENV_KEY_MAX];
	char		*str;
	char		c;
	int			i;
	t_env_entry	*env;

	i = 0;
	if (!exp)
		return (EXP_ERROR);
	exp->index++;
	c = exp->str[exp->index];
	if (c == '?')
	{
		str = ft_itoa(exp->mms->last_status);
		if (!str)
			return (EXP_ERROR);
		append_sb(exp->sb, str);
		free(str);
		exp->index++;
	}
	else if (c && c != ' ' && c != '\n' && (ft_isalnum(c) || c == '_'))
	{
		while (c && c != ' ' && c != '\n' && (ft_isalnum(c) || c == '_'))
		{
			if (i >= ENV_KEY_MAX - 1)
				return (EXP_LEN_VAR);
			buffer[i++] = exp->str[exp->index++];
			c = exp->str[exp->index];
		}
		buffer[i] = '\0';
		env = get_env(exp->mms->env, buffer);
		if (env)
			append_sb(exp->sb, env->value);
	}
	else
		append_ch_sb(exp->sb, '$');
	return (EXP_SUCCESS);
}

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

t_val_exp	exp_normal(t_exp *exp)
{
	char	c;

	if (!exp)
		return (EXP_ERROR);
	c = exp->str[exp->index];
	if (c == '\'')
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
	return (EXP_SUCCESS);
}

static t_val_exp	expand_word(t_exp *exp, t_exp_variant_fn *lut)
{
	t_val_exp	ret;

	if (!exp || !lut)
		return (EXP_ERROR);
	while (exp->str[exp->index])
	{
		ret = lut[exp->state](exp);
		if (ret != EXP_SUCCESS)
			return (ret);
	}
	return (EXP_SUCCESS);
}

static void	init_lut(t_exp_variant_fn *lut)
{
	if (!lut)
		return ;
	lut[ST_EXP_NORMAL] = exp_normal;
	lut[ST_EXP_SQUOTE] = exp_squote;
	lut[ST_EXP_DQUOTE] = exp_dquote;
}

t_val_exp	expand(t_mms *mms, t_tk ***tks)
{
	t_exp				*exp;
	t_tk				*tk;
	t_exp_variant_fn	lut[3];
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
		tk = (*tks)[i];
		if (tk->value)
		{
			reset_expander(exp, tk->value);
			if (tk->type_tk == TOK_WORD || tk->type_tk == TOK_DELIMITER)
			{
				if (expand_word(exp, lut))
				{
					if (!exp)
						return (0);
					if (exp->sb)
						free_sb(exp->sb);
					free(exp);
					return (0);
				}
				free((*tks)[i]->value);
				(*tks)[i]->value = ft_strdup(exp->sb->str);
			}
		}
		i++;
	}
	free_sb(exp->sb);
	free(exp);
	return (1);
}