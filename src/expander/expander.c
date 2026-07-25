/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:27:50 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/25 01:27:56 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

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

static t_val_exp	expand_token(t_exp *exp, t_tk *tk, t_exp_variant_fn *lut)
{
	char	*new_value;

	if (!tk->value)
		return (EXP_SUCCESS);
	reset_expander(exp, tk->value);
	if (tk->type_tk != TOK_WORD && tk->type_tk != TOK_DELIMITER)
		return (EXP_SUCCESS);
	if (expand_word(exp, lut))
		return (EXP_ERROR);
	new_value = ft_strdup(exp->sb->str);
	if (!new_value)
		return (EXP_ERROR);
	free(tk->value);
	tk->value = new_value;
	return (EXP_SUCCESS);
}

t_val_exp	expand(t_mms *mms, t_tk ***tks)
{
	t_exp				*exp;
	t_exp_variant_fn	lut[3];
	size_t				i;

	if (!mms || !tks || !*tks)
		return (EXP_ERROR);
	exp = init_expander(mms);
	if (!exp)
		return (EXP_ERROR);
	init_lut(lut);
	i = 0;
	while ((*tks)[i])
	{
		if (expand_token(exp, (*tks)[i], lut) == EXP_ERROR)
		{
			free_sb(exp->sb);
			free(exp);
			return (EXP_ERROR);
		}
		i++;
	}
	free_sb(exp->sb);
	free(exp);
	return (EXP_SUCCESS);
}
