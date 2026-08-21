/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_command.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:53:07 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/21 15:29:16 by fiaudfiz         ###   ########.fr       */
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

	if (!node->redirect)
		return (exec_builtin(mms, node));
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
	exec.cmd_path = get_cmd_path_child(mms, node, &exec);
	exit(execute(mms, node, &exec));
}

static int	wait_child(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		if (errno == EINTR)
			return (wait_child(pid));
		perror("miniMishell");
		return (1);
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

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
		perror("miniMishell");
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
