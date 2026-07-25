/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:32:57 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/25 01:33:45 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

void	init_lut(t_exp_variant_fn *lut)
{
	if (!lut)
		return ;
	lut[ST_EXP_NORMAL] = exp_normal;
	lut[ST_EXP_SQUOTE] = exp_squote;
	lut[ST_EXP_DQUOTE] = exp_dquote;
}

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

void	reset_expander(t_exp *exp, char *str)
{
	if (!exp || !str || !exp->sb)
		return ;
	exp->str = str;
	exp->index = 0;
	exp->state = ST_EXP_NORMAL;
	clear_sb(exp->sb);
}
