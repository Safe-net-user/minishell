/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_one.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 23:33:48 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/20 23:34:13 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

static void	free_expander(t_exp *exp)
{
	if (!exp)
		return ;
	free_sb(exp->sb);
	free(exp);
}

static t_val_exp	update_token_value(t_tk *tk, t_tk **tk_ref, t_exp *exp)
{
	char	*new_value;

	if (!exp->sb->str[0])
	{
		free(tk->value);
		tk->value = '\0';
		if (!(tk->flags & TOKF_SQUOTE)
			&& (!(tk->flags & TOKF_DQUOTE)
				&& tk->flags & TOKF_EXPANSION))
			unlink_token(tk_ref);
		return (EXP_SUCCESS);
	}
	new_value = ft_strdup(exp->sb->str);
	if (!new_value)
		return (EXP_ERROR);
	free(tk->value);
	tk->value = new_value;
	return (EXP_SUCCESS);
}

t_val_exp	expand_one(t_mms *mms, t_tk **tk_ref)
{
	t_exp				*exp;
	t_exp_variant_fn	lut[3];
	t_tk				*tk;

	tk = *tk_ref;
	if (!tk->value)
		return (EXP_SUCCESS);
	exp = init_expander(mms);
	if (!exp)
		return (EXP_ERROR);
	init_lut(lut);
	reset_expander(exp, tk->value);
	if (expand_word(exp, lut) != EXP_SUCCESS)
	{
		free_expander(exp);
		return (EXP_ERROR);
	}
	if (update_token_value(tk, tk_ref, exp) != EXP_SUCCESS)
	{
		free_expander(exp);
		return (EXP_ERROR);
	}
	free_expander(exp);
	return (EXP_SUCCESS);
}
