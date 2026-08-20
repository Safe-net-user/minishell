/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 14:43:40 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/20 14:42:08 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>

static void	free_tokens_values(t_tk *tok)
{
	while (tok)
	{
		free(tok->value);
		free(tok->heredoc_content);
		tok = tok->next;
	}
}

void	free_ast_values(t_ast *node)
{
	if (!node)
		return ;
	if (node->type == NODE_CMD)
	{
		free_tokens_values(node->tokens);
		free_tokens_values(node->redirect);
		return ;
	}
	free_ast_values(node->left);
	free_ast_values(node->right);
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

int	cleanup_and_exit(t_mms *mms)
{
	int	exit_status;

	if (mms->should_exit)
		exit_status = mms->exit_status;
	else
		exit_status = mms->last_status;
	close(mms->tty_fd);
	free_og_struct(mms);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	return (exit_status);
}
