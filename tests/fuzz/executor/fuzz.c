/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 13:56:54 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 14:04:47 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz.h"

void	init_fuzz_mms(t_mms *mms)
{
	memset(mms, 0, sizeof(*mms));
	mms->sa = init_stack_allocator(FUZZ_ARENA_SIZE);
	mms->cmd_path = init_hash_table(INIT_SIZE_HT);
	mms->alias = init_hash_table(INIT_SIZE_HT);
	set_new_value(mms->cmd_path, "PATH", "/usr/bin:/bin");
	mms->env = init_env(INIT_SIZE_HT);
	add_env(mms->env, "PATH", "/usr/bin:/bin", EXPORTED);
	mms->name = "minishell";
	mms->cwd = getcwd(NULL, 0);
	mms->last_status = 0;
}

void	free_fuzz_mms(t_mms *mms)
{
	if (mms->cwd)
		free(mms->cwd);
	if (mms->cmd_path)
		free_hash_table(mms->cmd_path);
	if (mms->alias)
		free_hash_table(mms->alias);
	if (mms->env)
		free_env(mms->env);
	if (mms->sa)
		stack_dealloc(mms->sa);
}

t_tk	*make_token(t_stack_alloc *sa, char *value, t_type_tk type)
{
	t_tk	*token;

	token = stack_alloc(sa, sizeof(t_tk));
	if (!token)
		return (NULL);
	token->value = value;
	token->flags = 0;
	token->type_tk = type;
	return (token);
}

t_ast	*make_cmd(t_stack_alloc *sa, char **argv)
{
	t_ast	*ast;
	size_t	i;

	ast = stack_alloc(sa, sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = NODE_CMD;
	ast->left = NULL;
	ast->right = NULL;
	ast->redirect = NULL;
	if (!argv)
	{
		ast->tokens = NULL;
		return (ast);
	}
	i = 0;
	while (argv[i])
		i++;
	ast->tokens = stack_alloc(sa, sizeof(t_tk *) * (i + 1));
	if (!ast->tokens)
		return (NULL);
	i = 0;
	while (argv[i])
	{
		ast->tokens[i] = make_token(sa, argv[i], TOK_WORD);
		i++;
	}
	ast->tokens[i] = NULL;
	return (ast);
}

t_ast	*make_cmd_raw(t_stack_alloc *sa, t_tk **tokens)
{
	t_ast	*ast;

	ast = stack_alloc(sa, sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = NODE_CMD;
	ast->left = NULL;
	ast->right = NULL;
	ast->redirect = NULL;
	ast->tokens = tokens;
	return (ast);
}

t_ast	*make_pipe(t_stack_alloc *sa, t_ast *left, t_ast *right)
{
	t_ast	*ast;

	ast = stack_alloc(sa, sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = NODE_PIPE;
	ast->left = left;
	ast->right = right;
	ast->redirect = NULL;
	ast->tokens = NULL;
	return (ast);
}

t_ast	*make_and(t_stack_alloc *sa, t_ast *left, t_ast *right)
{
	t_ast	*ast;

	ast = stack_alloc(sa, sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = NODE_AND;
	ast->left = left;
	ast->right = right;
	ast->redirect = NULL;
	ast->tokens = NULL;
	return (ast);
}

t_ast	*make_or(t_stack_alloc *sa, t_ast *left, t_ast *right)
{
	t_ast	*ast;

	ast = stack_alloc(sa, sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = NODE_OR;
	ast->left = left;
	ast->right = right;
	ast->redirect = NULL;
	ast->tokens = NULL;
	return (ast);
}

t_redir	*make_redir(t_stack_alloc *sa, t_type_tk type, char *file)
{
	t_redir	*redir;

	redir = stack_alloc(sa, sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type_tk = type;
	redir->file = file;
	redir->next = NULL;
	return (redir);
}

void	add_redir(t_ast *ast, t_redir *redir)
{
	t_redir	*current;

	if (!ast->redirect)
	{
		ast->redirect = redir;
		return ;
	}
	current = ast->redirect;
	while (current->next)
		current = current->next;
	current->next = redir;
}

static int	count_open_fds(void)
{
	DIR				*d;
	struct dirent	*entry;
	int				count;

	d = opendir("/proc/self/fd");
	if (!d)
		return (-1);
	count = 0;
	while ((entry = readdir(d)))
		if (entry->d_name[0] != '.')
			count++;
	closedir(d);
	return (count);
}

static void	fuzz_child(t_mms *mms, t_ast *ast, int pipe_fd)
{
	t_fuzz_result	partial;
	int				devnull;
	int				devnull_in;

	devnull_in = open("/dev/null", O_RDONLY);
	if (devnull_in >= 0)
	{
		dup2(devnull_in, STDIN_FILENO);
		close(devnull_in);
	}
	devnull = open("/dev/null", O_WRONLY);
	if (devnull >= 0)
	{
		dup2(devnull, STDOUT_FILENO);
		dup2(devnull, STDERR_FILENO);
		close(devnull);
	}
	memset(&partial, 0, sizeof(partial));
	partial.fd_before = count_open_fds();
	partial.exit_code = executor(mms, ast);
	partial.fd_after = count_open_fds();
	partial.outcome = FUZZ_OK;
	write(pipe_fd, &partial, sizeof(partial));
	close(pipe_fd);
	_exit(partial.exit_code & 0xff);
}

t_fuzz_result	run_fuzz_case(t_mms *mms, t_ast *ast)
{
	int				pipe_fd[2];
	pid_t			pid;
	pid_t			w;
	int				status;
	int				elapsed_ms;
	ssize_t			n;
	t_fuzz_result	result;
	t_fuzz_result	partial;

	memset(&result, 0, sizeof(result));
	if (pipe(pipe_fd) == -1)
	{
		result.outcome = FUZZ_CRASH;
		return (result);
	}
	pid = fork();
	if (pid == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		result.outcome = FUZZ_CRASH;
		return (result);
	}
	if (pid == 0)
	{
		close(pipe_fd[0]);
		fuzz_child(mms, ast, pipe_fd[1]);
	}
	close(pipe_fd[1]);
	elapsed_ms = 0;
	while (1)
	{
		w = waitpid(pid, &status, WNOHANG);
		if (w == pid)
			break ;
		if (elapsed_ms >= FUZZ_TIMEOUT_SEC * 1000)
		{
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
			close(pipe_fd[0]);
			result.outcome = FUZZ_TIMEOUT;
			return (result);
		}
		usleep(10000);
		elapsed_ms += 10;
	}
	if (WIFSIGNALED(status))
	{
		close(pipe_fd[0]);
		result.outcome = FUZZ_CRASH;
		result.signal_num = WTERMSIG(status);
		return (result);
	}
	n = read(pipe_fd[0], &partial, sizeof(partial));
	close(pipe_fd[0]);
	if (n != (ssize_t)sizeof(partial))
	{
		result.outcome = FUZZ_OK;
		result.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
		result.fd_before = -1;
		result.fd_after = -1;
		return (result);
	}
	result = partial;
	if (result.fd_before >= 0 && result.fd_after > result.fd_before)
		result.outcome = FUZZ_FD_LEAK;
	return (result);
}

void	print_fuzz_result(char *name, t_fuzz_result *res)
{
	if (res->outcome == FUZZ_OK)
		printf("[OK]      %-55s (exit=%d)\n", name, res->exit_code);
	else if (res->outcome == FUZZ_FD_LEAK)
		printf("[FDLEAK]  %-55s (%d -> %d fds ouverts)\n",
			name, res->fd_before, res->fd_after);
	else if (res->outcome == FUZZ_TIMEOUT)
		printf("[HANG]    %-55s (tué apres %ds)\n", name, FUZZ_TIMEOUT_SEC);
	else
		printf("[CRASH]   %-55s (signal %d: %s)\n",
			name, res->signal_num, strsignal(res->signal_num));
}