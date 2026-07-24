/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_1.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:49:46 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 23:46:22 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include <sys/wait.h>

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
	if (pipe(pipeline->fd) == -1)
		return (1);
	pipeline->pids[i] = fork();
	if (pipeline->pids[i] == -1)
	{
		perror("minishell");
		close(pipeline->fd[0]);
		close(pipeline->fd[1]);
		return (1);
	}
	if (pipeline->pids[i] == 0)
		pipeline_child(mms, pipeline, i);
	close(pipeline->fd[1]);
	if (pipeline->fd_prev != -1)
		close(pipeline->fd_prev);
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
		perror("minishell");
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
			perror("minishell");
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

/**
 * @brief Creates and executes the processes of a command pipeline.
 *
 * Creates the required pipes and child processes for each command except the
 * last one, connects each command's standard input to the previous pipe and
 * its standard output to the next pipe, then executes the last command with
 * the remaining input descriptor and the standard output.
 *
 * @param mms      Pointer to the main minishell structure.
 * @param node     AST node representing the root of the pipeline.
 * @param pipeline Pipeline execution context containing the commands, file
 *                 descriptors, and process identifiers.
 *
 * @return 1 if a pipe or fork operation fails, otherwise the exit status of
 *         the last command in the pipeline.
 */

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
