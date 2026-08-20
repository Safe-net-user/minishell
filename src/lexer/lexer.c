/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:33:11 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/20 16:04:08 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file lexer.c
 * @brief Entry point of the lexer — state-machine driven tokenizer.
 *
 * `lexer()` runs a 4-state automaton (LX_NORMAL, LX_SQUOTE, LX_DQUOTE,
 * LX_OPERATOR) over the raw command line, dispatching each character
 * through a lookup table of per-state handlers until the whole line
 * is consumed. Each handler advances `lx->index` and emits tokens
 * into the stack allocator as needed (see `emit_tk()`).
 *
 * `lx_end()` handles end-of-input: an unterminated quote at EOF is
 * reported as a specific error (`LX_SQUOTE_NF`/`LX_DQUOTE_NF`)
 * rather than silently closed, any pending word is flushed as a
 * final token, and a TOK_EOF sentinel is always emitted so the
 * parser has a well-defined end marker to stop on.
 */

#include "lexer.h"
#include "minishell.h"
#include <stdio.h>
#include "ft_string_builder.h"
#include "ft_strings.h"
#include <unistd.h>

static t_val_lx	lx_end(t_lx *lx)
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
