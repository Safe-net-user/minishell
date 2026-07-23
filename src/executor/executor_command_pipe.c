/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_command_pipe.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:45:31 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 22:36:05 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include <signal.h>

static int	setup_pipe_fds(int fd_in, int fd_out)
{
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		perror("minishell");
		return (1);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("minishell");
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
		exec->cmd_path = cmd->tokens[0]->value;
		return (0);
	}
	exec->cmd_path = find_path(mms, cmd, exec);
	if (exec->cmd_path == NULL)
	{
		print_cmd_not_found(cmd->tokens[0]->value);
		return (127);
	}
	return (0);
}

/**
 * @brief Executes a command within a pipeline.
 *
 * Redirects the command's standard input and output to the provided file
 * descriptors, applies command redirections and expansions, then executes
 * builtins or external commands according to the command path.
 *
 * Note: builtins running inside a pipeline stay forked (each pipeline stage
 * is its own subshell, same behaviour as real shells), unlike a standalone
 * builtin which now runs directly in execute_cmd().
 *
 * @param mms    Pointer to the main minishell structure.
 * @param cmd    AST node representing the command to execute.
 * @param fd_in  File descriptor used as the command's standard input.
 * @param fd_out File descriptor used as the command's standard output.
 *
 * @return The exit status of the executed builtin or external command, or a
 *         non-zero status if a redirection fails.
 */

int	execute_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out)
{
	t_executor	exec;
	int			status;

	signal(SIGPIPE, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (setup_pipe_fds(fd_in, fd_out) != 0)
		return (1);
	expand(mms, cmd->tokens);
	status = redirection(mms, cmd);
	if (status != 0)
		return (status);
	if (builtin(cmd))
		return (exec_builtin(mms, cmd));
	status = get_cmd_path(mms, cmd, &exec);
	if (status != 0)
		return (status);
	return (execute(mms, cmd, &exec));
}