/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_0.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:47:54 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 16:31:49 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "ft_strings.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/**
 * @brief Converts an array of token structures into an array of command strings.
 *
 * Allocates an array of strings using the stack allocator and stores the value
 * of each token in the corresponding position. The resulting array is
 * NULL-terminated and can be used as a command argument vector.
 *
 * @param[in] mms    Pointer to the minishell main structure.
 * @param[in] tokens NULL-terminated array of token pointers.
 *
 * @return A NULL-terminated array containing the values of the provided tokens.
 */

char	**tks_to_cmd_tab(t_mms *mms, t_tk **tokens)
{
	char	**cmd_tab;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (tokens[i])
		i++;
	cmd_tab = stack_alloc(mms->sa, sizeof(char *) * (i + 1));
	if (!cmd_tab)
		return (NULL);
	while (j < i)
	{
		cmd_tab[j] = tokens[j]->value;
		j++;
	}
	cmd_tab[j] = NULL;
	return (cmd_tab);
}

int	execute_redir_only(t_mms *mms, t_ast *node)
{
	int	saved_in;
	int	saved_out;
	int	status;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	status = redirection(mms, node);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (status);
}

int	exec_command(char *cmd_path, char **cmd_tab, char **envp)
{
	execve(cmd_path, cmd_tab, envp);
	if (errno == EACCES)
	{
		print_exec_error(cmd_path, "Permission denied");
		return (126);
	}
	if (errno == ENOENT)
	{
		print_exec_error(cmd_path, "No such file or directory");
		return (127);
	}
	print_exec_error(cmd_path, strerror(errno));
	return (126);
}
