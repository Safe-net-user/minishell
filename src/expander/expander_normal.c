/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_normal.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:31:21 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/25 01:31:46 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

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
