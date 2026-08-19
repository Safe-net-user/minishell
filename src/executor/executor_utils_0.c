/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_0.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:47:54 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/19 15:25:25 by gd-hallu         ###   ########.fr       */
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

int	save_std_fd(t_saved_fd *saved)
{
	saved->in = dup(STDIN_FILENO);
	saved->out = dup(STDOUT_FILENO);
	if (saved->in == -1 || saved->out == -1)
	{
		perror("miniMishell");
		if (saved->in != -1)
			close(saved->in);
		if (saved->out != -1)
			close(saved->out);
		return (0);
	}
	return (1);
}

void	restore_std_fd(t_saved_fd *saved)
{
	dup2(saved->in, STDIN_FILENO);
	dup2(saved->out, STDOUT_FILENO);
	close(saved->in);
	close(saved->out);
}

int	execute_redir_only(t_mms *mms, t_ast *node)
{
	t_saved_fd	saved;
	int			status;

	if (!save_std_fd(&saved))
		return (1);
	status = redirection(mms, node);
	restore_std_fd(&saved);
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
