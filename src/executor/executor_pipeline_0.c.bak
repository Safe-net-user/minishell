/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_0.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:58:40 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 17:50:25 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/**
 * @brief Counts the number of commands in a pipeline.
 *
 * Recursively traverses the left side of the pipeline AST and counts the
 * commands connected by pipe operators.
 *
 * @param[in] node AST node representing the pipeline.
 *
 * @return The number of commands contained in the pipeline.
 */

int	count_cmd_pipeline(t_ast *node)
{
	if (!node)
		return (0);
	if (node->type != NODE_PIPE)
		return (1);
	return (count_cmd_pipeline(node->left) + 1);
}

/**
 * @brief Fills an array with the commands contained in a pipeline.
 *
 * Recursively traverses the pipeline AST from left to right and stores each
 * command node in the provided array, preserving the execution order.
 *
 * @param[in]     node     Current AST node being traversed.
 * @param[out]    cmd_list Array receiving the command nodes of the pipeline.
 * @param[in,out] i        Current index in the command array.
 */

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

/**
 * @brief Creates an array containing all commands of a pipeline.
 *
 * Allocates an array of AST node pointers and fills it with the commands
 * extracted from the pipeline AST. The resulting array is NULL-terminated.
 *
 * @param[in] mms     Pointer to the minishell main structure.
 * @param[in] node    Root node of the pipeline AST.
 * @param[in] nb_cmd  Number of commands contained in the pipeline.
 *
 * @return A NULL-terminated array of command nodes on success, NULL if the
 *         allocation fails.
 */

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


/**
 * @brief Prepares and executes a pipeline of commands.
 *
 * Counts the number of commands in the pipeline, allocates the array of AST
 * command nodes and the array of process identifiers, then extracts each
 * command from the pipeline before delegating their execution.
 *
 * @param mms  Pointer to the main minishell structure.
 * @param node AST node representing the root of the pipeline.
 *
 * @return The exit status returned by the pipeline execution.
 */

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
