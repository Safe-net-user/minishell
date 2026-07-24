/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lx_dquote.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:54:43 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/24 23:54:54 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

t_val_lx	lx_dquote(t_lx *lx)
{
	char	c;

	c = lx->cmdl[lx->index];
	if (c == '\"')
	{
		append_ch_sb(lx->sb, c);
		lx->state = LX_NORMAL;
	}
	else if (c == '$')
	{
		lx->tk->flags |= TOKF_EXPANSION;
		append_ch_sb(lx->sb, c);
	}
	else
		append_ch_sb(lx->sb, c);
	lx->index++;
	return (LX_SUCCESS);
}
