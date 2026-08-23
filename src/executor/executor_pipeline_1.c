/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_1.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:49:46 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/23 17:35:44 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static void	pipeline_child(t_mms *mms, t_pipeline *pipeline, int i)
{
	int	fd_in;

	close(pipeline->fd[0]);
	if (i == 0)
		fd_in = STDIN_FILENO;
	else
		fd_in = pipeline->fd_prev;
	exit(execute_cmd_pipe(mms, pipeline->cmd_list[i],
			fd_in, pipeline->fd[1]));
}

static int	create_pipeline_process(t_mms *mms, t_pipeline *pipeline, int i)
{
	int	old_fd_prev;

	old_fd_prev = pipeline->fd_prev;
	if (pipe(pipeline->fd) == -1)
	{
		perror("miniMishell");
		return (1);
	}
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
	close(pipeline->fd[1]);
	if (old_fd_prev != -1)
		close(old_fd_prev);
	pipeline->fd_prev = pipeline->fd[0];
	return (0);
}

static int	create_last_process(t_mms *mms, t_pipeline *pipeline)
{
	int	i;

	i = pipeline->nb_cmd - 1;
	pipeline->pids[i] = fork();
	if (pipeline->pids[i] == -1)
	{
		perror("miniMishell");
		if (pipeline->fd_prev != -1)
			close(pipeline->fd_prev);
		return (1);
	}
	if (pipeline->pids[i] == 0)
		exit(execute_cmd_pipe(mms, pipeline->cmd_list[i],
				pipeline->fd_prev, STDOUT_FILENO));
	if (pipeline->fd_prev != -1)
		close(pipeline->fd_prev);
	return (0);
}

static int	wait_pipeline(t_pipeline *pipeline)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	while (i < pipeline->nb_cmd)
	{
		if (waitpid(pipeline->pids[i], &status, 0) == -1)
		{
			if (errno == EINTR)
				continue ;
			perror("miniMishell");
			return (1);
		}
		if (i == pipeline->nb_cmd - 1)
			last_status = status;
		i++;
	}
	if (WIFEXITED(last_status))
		return (WEXITSTATUS(last_status));
	if (WIFSIGNALED(last_status))
		return (128 + WTERMSIG(last_status));
	return (1);
}

int	execute_pipeline(t_mms *mms, t_ast *node, t_pipeline *pipeline)
{
	int	i;

	(void)node;
	i = 0;
	while (i < pipeline->nb_cmd - 1)
	{
		if (create_pipeline_process(mms, pipeline, i))
			return (1);
		i++;
	}
	if (create_last_process(mms, pipeline))
		return (1);
	return (wait_pipeline(pipeline));
}
