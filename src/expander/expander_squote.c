/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_squote.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:31:58 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/25 01:32:46 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

t_val_exp	exp_squote(t_exp *exp)
{
	if (!exp)
		return (EXP_ERROR);
	exp->index++;
	while (exp->str[exp->index] && exp->str[exp->index] != '\'')
	{
		append_ch_sb(exp->sb, exp->str[exp->index]);
		exp->index++;
	}
	if (exp->str[exp->index] == '\'')
		exp->index++;
	exp->state = ST_EXP_NORMAL;
	return (EXP_SUCCESS);
}
