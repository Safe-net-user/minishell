/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_command_pipe.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:45:31 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 14:15:49 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include <signal.h>
#include "expander.h"

static int	setup_pipe_fds(int fd_in, int fd_out)
{
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		perror("miniMishell");
		return (1);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("miniMishell");
		return (1);
	}
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
	return (0);
}

static int	get_cmd_path(t_mms *mms, t_ast *cmd, t_executor *exec)
{
	if (path_relative(cmd))
	{
		exec->cmd_path = cmd->tokens->value;
		return (0);
	}
	exec->cmd_path = find_path(mms, cmd, exec);
	if (exec->cmd_path == NULL)
	{
		print_cmd_not_found(cmd->tokens->value);
		return (127);
	}
	return (0);
}

static int	run_builtin_pipe(t_mms *mms, t_ast *cmd)
{
	int	status;

	status = exec_builtin(mms, cmd);
	if (mms->should_exit)
		return (mms->exit_status);
	return (status);
}

static int	prepare_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out)
{
	int	status;

	signal(SIGPIPE, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (setup_pipe_fds(fd_in, fd_out) != 0)
		return (-1);
	if (expand_tokens(mms, &cmd->tokens) == EXP_ERROR)
	{
		print_error("expansion failed");
		return (-1);
	}
	status = redirection(mms, cmd);
	if (status != 0)
		return (status);
	return (-2);
}

int	execute_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out)
{
	t_executor	exec;
	int			status;

	status = prepare_cmd_pipe(mms, cmd, fd_in, fd_out);
	if (status != -2)
		return (status);
	if (!cmd->tokens)
		return (0);
	if (builtin(cmd))
		return (run_builtin_pipe(mms, cmd));
	status = get_cmd_path(mms, cmd, &exec);
	if (status != 0)
		return (status);
	return (execute(mms, cmd, &exec));
}
