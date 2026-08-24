/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 14:32:16 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 19:04:46 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include "minishell.h"
#include "executor.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "ft_strings.h"

int	fork_and_run(t_mms *mms, t_ast *node)
{
	t_executor	exec;
	int			status;

	exec.pid = fork();
	if (exec.pid == -1)
	{
		perror("miniMishell");
		return (1);
	}
	if (exec.pid == 0)
		execute_child(mms, node);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	status = wait_child(exec.pid);
	set_signaux_interactif();
	tcsetattr(STDIN_FILENO, TCSADRAIN, mms->st);
	return (status);
}

char	**get_search_path(t_mms *mms)
{
	t_env_entry	*entry;

	entry = get_env(mms->env, "PATH");
	if (!entry)
		return (ft_split(".", ':'));
	if (!entry->value || !entry->value[0])
		return (NULL);
	return (ft_split(entry->value, ':'));
}

int	fork_pipeline_stage(t_mms *mms, t_pipeline *pipeline, int i,
		int old_fd_prev)
{
	pipeline->pids[i] = fork();
	if (pipeline->pids[i] == -1)
	{
		perror("miniMishell");
		close(pipeline->fd[0]);
		close(pipeline->fd[1]);
		return (1);
	}
	if (pipeline->pids[i] == 0)
	{
		pipeline->fd_prev = old_fd_prev;
		pipeline_child(mms, pipeline, i);
	}
	return (0);
}

void	print_signal_msg(int status)
{
	if (!WIFSIGNALED(status))
		return ;
	if (WTERMSIG(status) == SIGQUIT)
	{
		if (WCOREDUMP(status))
			write(STDERR_FILENO, "Quit (core dumped)\n", 20);
		else
			write(STDERR_FILENO, "Quit\n", 5);
	}
	else if (WTERMSIG(status) == SIGINT)
		write(STDERR_FILENO, "\n", 1);
}

void	restore_interactive_state(t_mms *mms)
{
	set_signaux_interactif();
	if (tcsetattr(STDIN_FILENO, TCSANOW, mms->st) == -1)
		perror("tcsetattr failed");
}
