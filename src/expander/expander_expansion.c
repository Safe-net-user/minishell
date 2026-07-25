/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:29:01 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/25 01:31:08 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

static t_val_exp	append_status_expansion(t_exp *exp)
{
	char	*str;

	str = ft_itoa(exp->mms->last_status);
	if (!str)
		return (EXP_ERROR);
	append_sb(exp->sb, str);
	free(str);
	exp->index++;
	return (EXP_SUCCESS);
}

static t_val_exp	append_var_expansion(t_exp *exp)
{
	char			buffer[ENV_KEY_MAX];
	char			c;
	int				i;
	t_env_entry		*env;

	i = 0;
	c = exp->str[exp->index];
	while (c && (ft_isalnum(c) || c == '_'))
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
	return (EXP_SUCCESS);
}

t_val_exp	exp_append_expansion(t_exp *exp)
{
	char	c;

	if (!exp)
		return (EXP_ERROR);
	exp->index++;
	c = exp->str[exp->index];
	if (c == '?')
		return (append_status_expansion(exp));
	if (c && (ft_isalnum(c) || c == '_'))
		return (append_var_expansion(exp));
	append_ch_sb(exp->sb, '$');
	return (EXP_SUCCESS);
}
