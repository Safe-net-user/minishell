/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_command.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:53:07 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/18 19:02:00 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "expander.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#define _POSIX_C_SOURCE 200809L	

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
	char		**cmd_tab;
	char		**envp;
	struct stat	st;

	if (!node->tokens->value)
	{
		print_cmd_not_found(node->tokens->value);
		exit(127);
	}
	cmd_tab = tks_to_cmd_tab(mms, node->tokens);
	if (!cmd_tab)
	{
		print_error("memory allocation failed");
		return (1);
	}
	if (stat(exec->cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		print_exec_error(exec->cmd_path, "Is a directory");
		return (126);
	}
	envp = env_to_envp(mms->env);
	return (exec_command(exec->cmd_path, cmd_tab, envp));
}

static int	execute_builtin(t_mms *mms, t_ast *node)
{
	t_saved_fd	saved;
	int			status;

	if (!save_std_fd(&saved))
		return (1);
	status = redirection(mms, node);
	if (status == 0)
		status = exec_builtin(mms, node);
	restore_std_fd(&saved);
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
	if (!node->tokens->value)
	{
		print_cmd_not_found(node->tokens->value);
		exit(127);
	}
	if (path_relative(node))
		exec.cmd_path = node->tokens->value;
	else
	{
		exec.cmd_path = find_path(mms, node, &exec);
		if (exec.cmd_path == NULL)
		{
			print_cmd_not_found(node->tokens->value);
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
		if (errno == EINTR)
			return (wait_child(pid));
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
	int			status;

	if (expand_tokens(mms, &node->tokens) == EXP_ERROR)
	{
		print_error("expansion failed");
		return (1);
	}
	if (!node->tokens)
		return (execute_redir_only(mms, node));
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
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	status = wait_child(exec.pid);
	set_signaux_interactif();
	return (status);
}
