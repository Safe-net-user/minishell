/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lx_squote.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:56:20 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/24 23:56:43 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

t_val_lx	lx_squote(t_lx *lx)
{
	char	c;

	c = lx->cmdl[lx->index];
	if (c == '\'')
	{
		append_ch_sb(lx->sb, c);
		lx->state = LX_NORMAL;
	}
	else
		append_ch_sb(lx->sb, c);
	lx->index++;
	return (LX_SUCCESS);
}
