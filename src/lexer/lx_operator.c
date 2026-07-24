/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lx_operator.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:55:58 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/24 23:56:10 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

static t_val_lx	lx_pipe(t_lx *lx)
{
	if (lx->cmdl[lx->index + 1] == '|')
	{
		lx->tk->type_tk = TOK_OR_IF;
		append_sb(lx->sb, "||");
		lx->index += 2;
	}
	else
	{
		lx->tk->type_tk = TOK_PIPE;
		append_ch_sb(lx->sb, '|');
		lx->index++;
	}
	if (!emit_tk(lx))
		return (LX_ERROR);
	return (LX_SUCCESS);
}

static t_val_lx	lx_ampersand(t_lx *lx)
{
	if (lx->cmdl[lx->index + 1] == '&')
	{
		lx->tk->type_tk = TOK_AND_IF;
		append_sb(lx->sb, "&&");
		lx->index += 2;
	}
	else
	{
		lx->tk->type_tk = TOK_AMPERSAND;
		append_ch_sb(lx->sb, '&');
		lx->index++;
	}
	if (!emit_tk(lx))
		return (LX_ERROR);
	return (LX_SUCCESS);
}

static t_val_lx	lx_less(t_lx *lx)
{
	if (lx->cmdl[lx->index + 1] == '<')
	{
		lx->tk->type_tk = TOK_DLESS;
		append_sb(lx->sb, "<<");
		if (!emit_tk(lx))
			return (LX_ERROR);
		lx->is_next_delimiter = true;
		lx->index += 2;
	}
	else
	{
		lx->tk->type_tk = TOK_LESS;
		append_ch_sb(lx->sb, '<');
		if (!emit_tk(lx))
			return (LX_ERROR);
		lx->index++;
	}
	return (LX_SUCCESS);
}

static t_val_lx	lx_great(t_lx *lx)
{
	if (lx->cmdl[lx->index + 1] == '>')
	{
		lx->tk->type_tk = TOK_DGREAT;
		append_sb(lx->sb, ">>");
		lx->index += 2;
	}
	else
	{
		lx->tk->type_tk = TOK_GREAT;
		append_ch_sb(lx->sb, '>');
		lx->index++;
	}
	if (!emit_tk(lx))
		return (LX_ERROR);
	return (LX_SUCCESS);
}

t_val_lx	lx_operator(t_lx *lx)
{
	char	c;

	c = lx->cmdl[lx->index];
	lx->state = LX_NORMAL;
	if (lx->sb->str[0] && !emit_tk(lx))
		return (LX_ERROR);
	if (c == '|')
		return (lx_pipe(lx));
	if (c == '&')
		return (lx_ampersand(lx));
	if (c == '<')
		return (lx_less(lx));
	if (c == '>')
		return (lx_great(lx));
	return (LX_ERROR);
}
