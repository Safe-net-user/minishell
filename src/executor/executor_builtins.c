/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_builtins.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:20:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 17:58:04 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "ft_strings.h"

/**
 * @brief Executes the builtin corresponding to a command.
 *
 * Identifies the builtin command from the first token of the AST node and
 * dispatches its execution to the corresponding builtin function.
 *
 * @param[in] mms  Pointer to the minishell main structure.
 * @param[in] node AST node containing the builtin command and its arguments.
 *
 * @return The exit status returned by the executed builtin.
 */

int	exec_builtin(t_mms *mms, t_ast *node)
{
	if (ft_strcmp(node->tokens[0]->value, "echo") == 0)
		return (builtin_echo(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "cd") == 0)
		return (builtin_cd(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "pwd") == 0)
		return (builtin_pwd(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "export") == 0)
		return (builtin_export(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "unset") == 0)
		return (builtin_unset(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "env") == 0)
		return (builtin_env(mms, node));
	else
		return (builtin_exit(mms, node));
}

/**
 * @brief Checks whether an AST node represents a builtin command.
 *
 * Compares the first token of the AST node against the list of supported
 * minishell builtin commands.
 *
 * @param[in] node AST node containing the command to check.
 *
 * @return 1 if the command is a builtin, 0 otherwise.
 */

int	builtin(t_ast *node)
{
	if (ft_strcmp(node->tokens[0]->value, "echo") == 0
		|| ft_strcmp(node->tokens[0]->value, "cd") == 0
		|| ft_strcmp(node->tokens[0]->value, "pwd") == 0
		|| ft_strcmp(node->tokens[0]->value, "export") == 0
		|| ft_strcmp(node->tokens[0]->value, "unset") == 0
		|| ft_strcmp(node->tokens[0]->value, "env") == 0
		|| ft_strcmp(node->tokens[0]->value, "exit") == 0)
		return (1);
	return (0);
}
