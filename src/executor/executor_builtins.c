/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_builtins.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:20:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/23 18:26:47 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "ft_strings.h"
#include "builtin.h"
#include <unistd.h>

int	exec_builtin(t_mms *mms, t_ast *node)
{
	if (ft_strcmp(node->tokens->value, "echo") == 0)
		return (builtin_echo(tks_to_cmd_tab(mms, node->tokens)));
	else if (ft_strcmp(node->tokens->value, "cd") == 0)
		return (builtin_cd(mms, tks_to_cmd_tab(mms, node->tokens)));
	else if (ft_strcmp(node->tokens->value, "pwd") == 0)
		return (builtin_pwd(mms));
	else if (ft_strcmp(node->tokens->value, "export") == 0)
		return (builtin_export(mms, tks_to_cmd_tab(mms, node->tokens)));
	else if (ft_strcmp(node->tokens->value, "unset") == 0)
		return (builtin_unset(mms, tks_to_cmd_tab(mms, node->tokens)));
	else if (ft_strcmp(node->tokens->value, "env") == 0)
		return (builtin_env(mms->env, tks_to_cmd_tab(mms, node->tokens)));
	else
	{
		if (mms->is_pipeline != 1)
			write(STDOUT_FILENO, "exit\n", 5);
		return (builtin_exit(mms, tks_to_cmd_tab(mms, node->tokens)));
	}
}

int	builtin(t_ast *node)
{
	if (!node->tokens || !node->tokens->value)
		return (0);
	if (ft_strcmp(node->tokens->value, "echo") == 0
		|| ft_strcmp(node->tokens->value, "cd") == 0
		|| ft_strcmp(node->tokens->value, "pwd") == 0
		|| ft_strcmp(node->tokens->value, "export") == 0
		|| ft_strcmp(node->tokens->value, "unset") == 0
		|| ft_strcmp(node->tokens->value, "env") == 0
		|| ft_strcmp(node->tokens->value, "exit") == 0)
		return (1);
	return (0);
}
