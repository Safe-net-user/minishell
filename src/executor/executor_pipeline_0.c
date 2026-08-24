/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_0.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:58:40 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 19:16:53 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int	count_cmd_pipeline(t_ast *node)
{
	if (!node)
		return (0);
	if (node->type != NODE_PIPE)
		return (1);
	return (count_cmd_pipeline(node->left) + 1);
}

void	fill_pipeline_cmds(t_ast *node, t_ast **cmd_list, int *i)
{
	if (!node)
		return ;
	if (node->type == NODE_PIPE)
	{
		fill_pipeline_cmds(node->left, cmd_list, i);
		cmd_list[*i] = node->right;
		(*i)++;
	}
	else
	{
		cmd_list[*i] = node;
		(*i)++;
	}
}

t_ast	**add_cmd_pipeline(t_mms *mms, t_ast *node, int nb_cmd)
{
	t_ast	**cmd_list;
	int		i;

	cmd_list = stack_alloc(mms->sa, sizeof(t_ast *) * (nb_cmd + 1));
	if (!cmd_list)
		return (NULL);
	i = 0;
	fill_pipeline_cmds(node, cmd_list, &i);
	cmd_list[i] = NULL;
	return (cmd_list);
}

int	pipeline(t_mms *mms, t_ast *node)
{
	t_pipeline	*pipeline;

	pipeline = stack_alloc(mms->sa, sizeof(t_pipeline));
	if (!pipeline)
	{
		print_error("memory allocation failed");
		return (1);
	}
	pipeline->nb_cmd = count_cmd_pipeline(node);
	pipeline->cmd_list = add_cmd_pipeline(mms, node,
			pipeline->nb_cmd);
	if (!pipeline->cmd_list)
	{
		print_error("memory allocation failed");
		return (1);
	}
	pipeline->pids = stack_alloc(mms->sa,
			sizeof(pid_t) * pipeline->nb_cmd);
	if (!pipeline->pids)
	{
		print_error("memory allocation failed");
		return (1);
	}
	pipeline->fd_prev = -1;
	return (execute_pipeline(mms, node, pipeline));
}

int	pipeline_error(t_mms *mms)
{
	restore_interactive_state(mms);
	mms->is_pipeline = 0;
	return (1);
}
