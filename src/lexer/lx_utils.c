/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lx_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 00:06:45 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/25 03:11:25 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"
#include <stdio.h>
#include "ft_string_builder.h"
#include "ft_strings.h"
#include <unistd.h>

int	emit_tk(t_lx *lx)
{
	t_tk	*tk;

	if (!lx)
		return (0);
	tk = stack_alloc(lx->mms->sa, sizeof(t_tk));
	if (!tk)
		return (0);
	if (lx->tk->type_tk == TOK_PIPE || lx->tk->type_tk == TOK_AND_IF
		|| lx->tk->type_tk == TOK_OR_IF || lx->tk->type_tk == TOK_AMPERSAND
		|| lx->tk->type_tk == TOK_LESS || lx->tk->type_tk == TOK_GREAT
		|| lx->tk->type_tk == TOK_DGREAT || lx->tk->type_tk == TOK_DLESS)
		tk->value = NULL;
	else
	{
		tk->value = ft_strdup(lx->sb->str);
		if (!tk->value)
			return (0);
	}
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

int	emit_eof(t_lx *lx)
{
	t_tk	*tk;

	tk = stack_alloc(lx->mms->sa, sizeof(t_tk));
	if (!tk)
		return (0);
	tk->value = "";
	tk->flags = 0;
	tk->type_tk = TOK_EOF;
	return (1);
}

void	fill_lx(t_lx *lx, char *cmdl, t_mms *mms, t_tk *tk)
{
	lx->cmdl = cmdl;
	lx->state = LX_NORMAL;
	lx->index = 0;
	lx->mms = mms;
	lx->tk = tk;
	lx->tk->value = NULL;
	lx->tk->flags = 0;
	lx->tk->type_tk = 0;
}

t_lx	*init_s_lx( char *cmdl, t_mms *mms)
{
	t_lx	*lx;
	t_tk	*tk;

	lx = malloc(sizeof(t_lx));
	if (!lx)
		return (0);
	tk = malloc(sizeof(t_tk));
	if (!tk)
	{
		free(lx);
		return (0);
	}
	fill_lx(lx, cmdl, mms, tk);
	lx->sb = init_sb(DEFAULT_SB_SIZE);
	lx->is_next_delimiter = 0;
	if (!lx->sb)
	{
		free(lx->tk);
		free(lx);
		return (NULL);
	}
	return (lx);
}

void	free_lexer(t_lx *lx)
{
	if (!lx)
		return ;
	free(lx->tk);
	free_sb(lx->sb);
	free(lx);
}
