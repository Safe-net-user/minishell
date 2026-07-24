/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:33:11 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 11:13:22 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"
#include <stdio.h>
#include "ft_string_builder.h"
#include "ft_strings.h"
#include <unistd.h>

int emit_tk(t_lx *lx)
{
    t_tk *tk;

    if (!lx)
        return (0);
    tk = stack_alloc(lx->mms->sa, sizeof(t_tk));
    if (!tk)
        return (0);
    tk->value = ft_strdup(lx->sb->str);
    tk->flags = lx->tk->flags;
    if (lx->is_next_delimiter)
        lx->tk->type_tk = TOK_DELIMITER;
    lx->is_next_delimiter = 0;
    tk->type_tk = lx->tk->type_tk;
    clear_sb(lx->sb);
    lx->tk->flags = 0;
    lx->tk->type_tk = TOK_WORD;
    return (1);
}

int emit_eof(t_lx *lx)
{
    t_tk *tk;

    tk = stack_alloc(lx->mms->sa, sizeof(t_tk));
    if (!tk)
        return (0);
    tk->value = "";
    tk->flags = 0;
    tk->type_tk = TOK_EOF;
    return (1);
}

t_lx *init_s_lx( char *cmdl, t_mms *mms)
{
    t_lx *lx;
    t_tk *tk;

    lx = malloc(sizeof(t_lx));
    if (!lx)
        return (0);
    tk = malloc(sizeof(t_tk));
    if (!tk)
    {
        free(lx);
        return (0);
    }
    lx->cmdl = cmdl;
    lx->state = LX_NORMAL;
    lx->index = 0;
    lx->mms = mms;
    lx->tk = tk;
    lx->tk->value = NULL;
    lx->tk->flags = 0;
    lx->tk->type_tk = 0;
    lx->sb = init_sb(DEFAULT_SB_SIZE);
    lx->is_next_delimiter = 0;
    if (!lx->sb)
        return (NULL);
    return (lx);
}

/**
 * @brief Loops through the command line to determine the rule associated
 * with each character.
 *
 * This function processes the command line read from the REPL (readline).
 * Using a greedy algorithm, it selects the first rule that matches the
 * current character. Characters are appended while the active rule does
 * not request parsing to stop.
 */
void	free_lexer(t_lx *lx)
{
	if (!lx)
		return ;
	free(lx->tk);
	free_sb(lx->sb);
	free(lx);
}

t_val_lx	lexer(char *cmdl, t_mms *mms)
{
	t_lx		*lx;
	t_val_lx	value;
	state_lx_fn	state_lx_fn_lut[4];

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
		if (!emit_eof(lx))
		{
			free_lexer(lx);
			return (LX_ERROR);
		}
	}
	free_lexer(lx);
	return (LX_SUCCESS);
}