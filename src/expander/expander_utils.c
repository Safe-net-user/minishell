/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:32:57 by gaspard           #+#    #+#             */
/*   Updated: 2026/08/20 23:48:13 by gd-hallu         ###   ########.fr       */
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

/*
** Detache *tk_ref de sa liste chainee et met a jour tk_ref pour
** pointer sur le token suivant (ou NULL). Met a jour prev/next
** des voisins pour garder la liste coherente.
*/
void	unlink_token(t_tk **tk_ref)
{
	t_tk	*tk;
	t_tk	*next;
	t_tk	*prev;

	tk = *tk_ref;
	next = tk->next;
	prev = tk->prev;
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	*tk_ref = next;
}
