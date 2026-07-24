/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:33:11 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/25 00:06:27 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"
#include <stdio.h>
#include "ft_string_builder.h"
#include "ft_strings.h"
#include <unistd.h>

static t_lx	*lx_end(t_lx *lx)
{
	if (lx->state == LX_SQUOTE)
	{
		free_lexer(lx);
		return (LX_SQUOTE_NF);
	}
	if (lx->state == LX_DQUOTE)
	{
		free_lexer(lx);
		return (LX_DQUOTE_NF);
	}
	if (lx->sb->str[0] != '\0')
	{
		if (!emit_tk(lx))
		{
			free_lexer(lx);
			return (LX_ERROR);
		}
	}
	if (!emit_eof(lx))
	{
		free_lexer(lx);
		return (LX_ERROR);
	}
	free_lexer(lx);
	return (LX_SUCCESS);
}

t_val_lx	lexer(char *cmdl, t_mms *mms)
{
	t_lx			*lx;
	t_val_lx		value;
	t_state_lx_fn	state_lx_fn_lut[4];

	if (!mms || !cmdl)
		return (LX_ERROR);
	lx = init_s_lx(cmdl, mms);
	if (!lx)
		return (LX_ERROR);
	state_lx_fn_lut[0] = lx_normal;
	state_lx_fn_lut[1] = lx_squote;
	state_lx_fn_lut[2] = lx_dquote;
	state_lx_fn_lut[3] = lx_operator;
	while (lx->cmdl[lx->index])
	{
		value = state_lx_fn_lut[lx->state](lx);
		if (value != LX_SUCCESS)
		{
			free_lexer(lx);
			return (value);
		}
	}
	return (lx_end(lx));
}
