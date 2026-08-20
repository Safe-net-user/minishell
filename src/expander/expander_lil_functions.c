/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_lil_functions.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 22:27:21 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/20 23:49:22 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

t_val_exp	expand_word_failed(t_exp *exp)
{
	free_sb(exp->sb);
	free(exp);
	return (EXP_ERROR);
}

void	empty_token(t_tk *tk, t_tk **tk_ref)
{
	free(tk->value);
	tk->value = '\0';
	if (!(tk->flags & TOKF_SQUOTE) && (!(tk->flags & TOKF_DQUOTE) \
&& tk->flags & TOKF_EXPANSION))
		unlink_token(tk_ref);
}

t_val_exp	new_value_failed(t_exp *exp)
{
	free_sb(exp->sb);
	free(exp);
	return (EXP_ERROR);
}

t_val_exp	end_expand_one(t_exp *exp)
{
	free_sb(exp->sb);
	free(exp);
	return (EXP_SUCCESS);
}

void	free_fill(t_tk *tk, char *str)
{
	free(tk->value);
	tk->value = str;
}
