/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_0.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:47:54 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/12 12:00:12 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "ft_strings.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

char	**tks_to_cmd_tab(t_mms *mms, t_tk *tokens)
{
	char	**cmd_tab;
	t_tk	*tmp;
	int		count;
	int		j;

	count = count_tokens(tokens);
	cmd_tab = stack_alloc(mms->sa, sizeof(char *) * (count + 1));
	if (!cmd_tab)
		return (NULL);
	tmp = tokens;
	j = 0;
	while (j < count)
	{
		cmd_tab[j] = tmp->value;
		tmp = tmp->next;
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
