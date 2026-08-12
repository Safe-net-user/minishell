/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 14:43:40 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/12 11:57:52 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	stack_reset(t_stack_alloc *sa)
{
	sa->up = sa->buffer;
	sa->curr = NULL;
}

void	free_ast_values(t_ast *node)
{
	t_tk	*tok;

	if (!node)
		return ;
	if (node->type == NODE_CMD)
	{
		tok = node->tokens;
		while (tok)
		{
			free(tok->value);
			tok = tok->next;
		}
		tok = node->redirect;
		while (tok)
		{
			free(tok->value);
			tok = tok->next;
		}
	}
	else
	{
		free_ast_values(node->left);
		free_ast_values(node->right);
	}
}

void	free_stack_allocator(t_stack_alloc *sa)
{
	if (!sa)
		return ;
	free(sa->buffer);
	free(sa);
}

void	free_og_struct(t_mms *mms)
{
	if (!mms)
		return ;
	free_hash_table(mms->alias);
	free_hash_table(mms->cmd_path);
	free_env(mms->env);
	free_stack_allocator(mms->sa);
	free(mms->cwd);
	free(mms->name);
	free(mms);
}
