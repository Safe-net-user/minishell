/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 14:43:40 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 11:36:12 by fiaudfiz         ###   ########.fr       */
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
	t_redir	*redir;
	int		i;

	if (!node)
		return ;
	if (node->type == NODE_CMD)
	{
		i = 0;
		while (node->tokens[i])
		{
			free(node->tokens[i]->value);
			i++;
		}
		redir = node->redirect;
		while (redir)
		{
			free(redir->file);
			redir = redir->next;
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

void    free_og_struct(t_mms *mms)
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