/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_command.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:53:07 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 00:06:02 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "expander.h"
#include <sys/wait.h>

/**
 * @brief Executes an external command using execve().
 *
 * Executes the command using either its directly specified path or the path
 * resolved by the command lookup mechanism. If execve() fails, errno is used
 * to tell a permission error (126) apart from a missing command (127).
 *
 * @param mms  Pointer to the main minishell structure.
 * @param node AST node representing the command to execute.
 * @param exec Execution context containing the resolved command path.
 *
 * @return 126 if the command exists but is not executable, 127 if execve()
 *         fails for any other reason (command not found, etc).
 */

int	execute(t_mms *mms, t_ast *node, t_executor *exec)
{
	char	**cmd_tab;

	cmd_tab = tks_to_cmd_tab(mms, node->tokens);
	if (!cmd_tab)
	{
		print_error("memory allocation failed");
		return (1);
	}
	execve(exec->cmd_path, cmd_tab, hash_table_to_envp(mms->env));
	perror("minishell");
	if (errno == EACCES)
		return (126);
	return (127);
}


static int	execute_builtin(t_mms *mms, t_ast *node)
{
	int	saved_in;
	int	saved_out;
	int	status;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	status = redirection(mms, node);
	if (status == 0)
		status = exec_builtin(mms, node);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (status);
}

static void	execute_child(t_mms *mms, t_ast *node)
{
	t_executor	exec;
	int			status;

	signal(SIGPIPE, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	status = redirection(mms, node);
	if (status != 0)
		exit(status);
	if (path_relative(node))
		exec.cmd_path = node->tokens[0]->value;
	else
	{
		exec.cmd_path = find_path(mms, node, &exec);
		if (exec.cmd_path == NULL)
		{
			print_cmd_not_found(node->tokens[0]->value);
			exit(127);
		}
	}
	exit(execute(mms, node, &exec));
}

static int	wait_child(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		perror("minishell");
		return (1);
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

/**
 * @brief Executes a single command.
 *
 * Builtins now run directly in the current process (no fork), so that
 * cd/export/unset/exit actually affect the shell's own state. Redirections
 * are applied to saved copies of stdin/stdout and restored right after the
 * builtin returns. External commands are still executed in a forked child.
 *
 * @param mms  Pointer to the main minishell structure.
 * @param node AST node representing the command to execute.
 *
 * @return The status of the fork operation, the builtin's exit status, or
 *         the command execution status in the child process.
 */

int	execute_cmd(t_mms *mms, t_ast *node)
{
	t_executor	exec;

	expand(mms, &node->tokens);
	if (builtin(node))
		return (execute_builtin(mms, node));
	exec.pid = fork();
	if (exec.pid == -1)
	{
		perror("minishell");
		return (1);
	}
	if (exec.pid == 0)
		execute_child(mms, node);
	return (wait_child(exec.pid));
}
