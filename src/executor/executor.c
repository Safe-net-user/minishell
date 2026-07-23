/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:20:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 14:40:39 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "ft_strings.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h> // CHANGE: needed to distinguish EACCES (126) / ENOENT (127) after execve
#include "gnl.h"
#include <signal.h>

static void	print_error(char *msg)
{
	write(2, "minishell: ", 11);
	write(2, msg, ft_strlen(msg));
	write(2, "\n", 1);
}

static void	print_cmd_not_found(char *cmd)
{
	write(2, "minishell: ", 11);
	write(2, cmd, ft_strlen(cmd));
	write(2, ": command not found\n", 20);
}

/**
 * @brief Reads a line from standard input and writes it to the here-document pipe.
 *
 * Prompts the user for input, reads a line from standard input, and compares it
 * with the here-document delimiter. If the delimiter is reached or an end-of-file
 * is encountered, the function stops reading.
 *
 * @param[in]  fd_here_doc File descriptors of the here-document pipe.
 * @param[in]  lim_nl      Delimiter marking the end of the here-document.
 *
 * @return 1 if the delimiter is reached or EOF is encountered, 0 otherwise,
 *         -1 if the write to the pipe fails.
 */

int	execute_here_doc(int *fd_here_doc, char *lim_nl)
{
	char	*line;

	fprintf(stdout, "> ");
	line = get_next_line(0);
	if (!line || ft_strcmp(line, lim_nl) == 0)
	{
		free(line);
		return (1);
	}
	// CHANGE: write() return value was never checked, failure was silently ignored
	if (write(fd_here_doc[1], line, ft_strlen(line)) == -1)
	{
		perror("minishell"); // CHANGE
		free(line);
		return (-1);
	}
	free(line);
	return (0);
}

/**
 * @brief Reads and stores here-document input into a pipe.
 *
 * Creates a pipe and continuously reads lines from standard input until the
 * here-document delimiter is encountered or EOF is reached. The input is
 * written to the pipe, whose read end is returned for later use as standard input.
 *
 * @param[in]  mms  Pointer to the minishell main structure.
 * @param[in]  redir Here-document redirection containing the delimiter.
 *
 * @return The read end of the here-document pipe on success, -1 on error.
 */

int	here_doc(t_mms *mms, t_redir *redir)
{
	int		fd_here_doc[2];
	char	*lim_nl;
	int		res;

	(void)mms;
	if (pipe(fd_here_doc))
	{
		perror("minishell"); // CHANGE
		return (-1);
	}
	lim_nl = ft_strjoin(redir->file, "\n");
	if (!lim_nl)
	{
		print_error("memory allocation failed"); // CHANGE
		close(fd_here_doc[0]);
		close(fd_here_doc[1]);
		return (-1);
	}
	while (1)
	{
		res = execute_here_doc(fd_here_doc, lim_nl);
		// CHANGE: propagate a write() failure instead of looping/treating it as EOF
		if (res == -1)
		{
			get_next_line(-1);
			free(lim_nl);
			close(fd_here_doc[0]);
			close(fd_here_doc[1]);
			return (-1);
		}
		if (res == 1)
			break ;
	}
	get_next_line(-1);
	free(lim_nl);
	close(fd_here_doc[1]);
	return (fd_here_doc[0]);
}

/**
 * @brief Converts an array of token structures into an array of command strings.
 *
 * Allocates an array of strings using the stack allocator and stores the value
 * of each token in the corresponding position. The resulting array is
 * NULL-terminated and can be used as a command argument vector.
 *
 * @param[in] mms    Pointer to the minishell main structure.
 * @param[in] tokens NULL-terminated array of token pointers.
 *
 * @return A NULL-terminated array containing the values of the provided tokens.
 */

char	**tks_to_cmd_tab(t_mms *mms, t_tk **tokens)
{
	char	**cmd_tab;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (tokens[i])
		i++;
	cmd_tab = stack_alloc(mms->sa, sizeof(char *) * (i + 1));
	if (!cmd_tab) // CHANGE: allocation failure was never checked before writing into cmd_tab
		return (NULL);
	while (j < i)
	{
		cmd_tab[j] = tokens[j]->value;
		j++;
	}
	cmd_tab[j] = NULL;
	return (cmd_tab);
}

/**
 * @brief Redirects standard input from a file.
 *
 * Opens the input file specified by the redirection and replaces the current
 * standard input file descriptor with the opened file.
 *
 * @param[in] mms   Pointer to the minishell main structure.
 * @param[in] redir Input redirection containing the file path.
 *
 * @return 0 on success, 1 if the file cannot be opened or standard input
 *         cannot be redirected.
 */

int	redirection_in(t_mms *mms, t_redir *redir)
{
	int	fd_in;

	(void)mms;
	fd_in = open(redir->file, O_RDONLY);
	if (fd_in == -1)
	{
		perror("minishell");
		return (1);
	}
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		perror("minishell");
		close(fd_in);
		return (1);
	}
	close(fd_in);
	return (0);
}

/**
 * @brief Redirects standard output to a file.
 *
 * Opens the output file in append mode for the `>>` operator or truncation mode
 * for the `>` operator, then replaces the current standard output file
 * descriptor with the opened file.
 *
 * @param[in] mms   Pointer to the minishell main structure.
 * @param[in] redir Output redirection containing the file path and redirection type.
 *
 * @return 0 on success, 1 if the file cannot be opened or standard output
 *         cannot be redirected.
 */

int	redirection_out(t_mms *mms, t_redir *redir)
{
	int	fd_out;

	(void)mms;
	if (redir->type_tk == TOK_DGREAT)
		fd_out = open(redir->file,
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd_out = open(redir->file,
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out == -1)
	{
		perror("minishell");
		return (1);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("minishell");
		close(fd_out);
		return (1);
	}
	close(fd_out);
	return (0);
}

/**
 * @brief Applies all redirections associated with an AST node.
 *
 * Iterates through the list of redirections attached to the node and applies
 * each one according to its type: here-document, input redirection, or output
 * redirection. Stops immediately if a redirection fails.
 *
 * @param[in] mms  Pointer to the minishell main structure.
 * @param[in] node AST node containing the redirections to apply.
 *
 * @return 0 if all redirections are successfully applied, otherwise the error
 *         status returned by the failed redirection.
 */

int	redirection(t_mms *mms, t_ast *node)
{
	t_redir	*redir;
	int		status;
	int		fd_heredoc;

	redir = node->redirect;
	while (redir != NULL)
	{
		if (redir->type_tk == TOK_DLESS)
		{
			// CHANGE: was calling an undefined `heredoc()` and treating the
			// returned fd as if it were a status. here_doc() returns a fd
			// (or -1), so it must be dup2'd onto stdin like any other
			// input redirection.
			fd_heredoc = here_doc(mms, redir);
			if (fd_heredoc == -1)
				return (1);
			if (dup2(fd_heredoc, STDIN_FILENO) == -1)
			{
				perror("minishell");
				close(fd_heredoc);
				return (1);
			}
			close(fd_heredoc);
			status = 0;
		}
		else if (redir->type_tk == TOK_LESS)
			status = redirection_in(mms, redir);
		else
			status = redirection_out(mms, redir);
		if (status != 0)
			return (status);
		redir = redir->next;
	}
	return (0);
}

/**
 * @brief Executes the builtin corresponding to a command.
 *
 * Identifies the builtin command from the first token of the AST node and
 * dispatches its execution to the corresponding builtin function.
 *
 * @param[in] mms  Pointer to the minishell main structure.
 * @param[in] node AST node containing the builtin command and its arguments.
 *
 * @return The exit status returned by the executed builtin.
 */
/*
int	exec_builtin(t_mms *mms, t_ast *node)
{
	if (ft_strcmp(node->tokens[0]->value, "echo") == 0)
		return (builtin_echo(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "cd") == 0)
		return (builtin_cd(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "pwd") == 0)
		return (builtin_pwd(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "export") == 0)
		return (builtin_export(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "unset") == 0)
		return (builtin_unset(mms, node));
	else if (ft_strcmp(node->tokens[0]->value, "env") == 0)
		return (builtin_env(mms, node));
	else
		return (builtin_exit(mms, node));
}*/

/**
 * @brief Checks whether an AST node represents a builtin command.
 *
 * Compares the first token of the AST node against the list of supported
 * minishell builtin commands.
 *
 * @param[in] node AST node containing the command to check.
 *
 * @return 1 if the command is a builtin, 0 otherwise.
 */
/*
int	builtin(t_ast *node)
{
	if (ft_strcmp(node->tokens[0]->value, "echo") == 0
		|| ft_strcmp(node->tokens[0]->value, "cd") == 0
		|| ft_strcmp(node->tokens[0]->value, "pwd") == 0
		|| ft_strcmp(node->tokens[0]->value, "export") == 0
		|| ft_strcmp(node->tokens[0]->value, "unset") == 0
		|| ft_strcmp(node->tokens[0]->value, "env") == 0
		|| ft_strcmp(node->tokens[0]->value, "exit") == 0)
		return (1);
	return (0);
}*/

/**
 * @brief Counts the number of commands in a pipeline.
 *
 * Recursively traverses the left side of the pipeline AST and counts the
 * commands connected by pipe operators.
 *
 * @param[in] node AST node representing the pipeline.
 *
 * @return The number of commands contained in the pipeline.
 */

int	count_cmd_pipeline(t_ast *node)
{
	if (!node)
		return (0);
	if (node->type != NODE_PIPE)
		return (1);
	return (count_cmd_pipeline(node->left) + 1);
}

/**
 * @brief Fills an array with the commands contained in a pipeline.
 *
 * Recursively traverses the pipeline AST from left to right and stores each
 * command node in the provided array, preserving the execution order.
 *
 * @param[in]     node     Current AST node being traversed.
 * @param[out]    cmd_list Array receiving the command nodes of the pipeline.
 * @param[in,out] i        Current index in the command array.
 */

static void	fill_pipeline_cmds(t_ast *node, t_ast **cmd_list, int *i)
{
	if (!node)
		return ;
	if (node->type == NODE_PIPE)
	{
		fill_pipeline_cmds(node->left, cmd_list, i);
		cmd_list[*i] = node->right;
		(*i)++;
	}
	else
	{
		cmd_list[*i] = node;
		(*i)++;
	}
}

/**
 * @brief Creates an array containing all commands of a pipeline.
 *
 * Allocates an array of AST node pointers and fills it with the commands
 * extracted from the pipeline AST. The resulting array is NULL-terminated.
 *
 * @param[in] mms     Pointer to the minishell main structure.
 * @param[in] node    Root node of the pipeline AST.
 * @param[in] nb_cmd  Number of commands contained in the pipeline.
 *
 * @return A NULL-terminated array of command nodes on success, NULL if the
 *         allocation fails.
 */

t_ast	**add_cmd_pipeline(t_mms *mms, t_ast *node, int nb_cmd)
{
	t_ast	**cmd_list;
	int		i;

	cmd_list = stack_alloc(mms->sa, sizeof(t_ast *) * (nb_cmd + 1));
	if (!cmd_list)
		return (NULL);
	i = 0;
	fill_pipeline_cmds(node, cmd_list, &i);
	cmd_list[i] = NULL;
	return (cmd_list);
}

/**
 * @brief Frees a NULL-terminated array of strings produced by ft_split.
 *
 * @param[in] arr NULL-terminated array of strings to free.
 */
// CHANGE: new helper, needed to fix the find_path() leak (path array from
// ft_split was never freed).
static void	free_path_arr(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/**
 * @brief Searches for a command executable in the PATH environment variable.
 *
 * Splits the PATH variable into its individual directories and checks each
 * directory for an executable matching the command name. The first valid
 * executable path found is returned.
 *
 * @param mms  Pointer to the main minishell structure containing the
 *             environment data.
 * @param node AST node representing the command to search for.
 * @param exec Execution context associated with the command lookup.
 *
 * @return The full path to the executable if found, otherwise NULL.
 */

char	*find_path(t_mms *mms, t_ast *node, t_executor *exec)
{
	char	**path;
	char	*temp;
	char	*result;
	int		i;

	(void)exec;
	i = 0;
	result = NULL;
	path = ft_split(get_pointer(mms->cmd_path, "PATH"), ':');
	if (!path || !path[0])
	{
		free_path_arr(path); // CHANGE: free the split array before bailing out
		return (NULL);
	}
	while (path[i] && !result)
	{
		temp = ft_strjoin(path[i], "/");
		if (!temp)
			break ;
		temp = ft_strjoin(temp, node->tokens[0]->value); //join and free mode 1
		if (!temp)
			break ;
		if (access(temp, F_OK | X_OK) == 0)
			result = temp;
		else
			free(temp);
		i++;
	}
	free_path_arr(path); // CHANGE: path array is now always freed, fixes the leak
	return (result);
}

/**
 * @brief Checks whether a command specifies a path explicitly.
 *
 * Determines whether the command name contains a slash, indicating that the
 * command should be executed using the path provided by the user rather than
 * being searched for in the PATH environment variable.
 *
 * @param node AST node representing the command to inspect.
 *
 * @return 1 if the command contains a slash, otherwise 0.
 */

int	path_relative(t_ast *node)
{
	if (ft_strchr(node->tokens[0]->value, '/'))
		return (1);
	return (0);
}

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
	cmd_tab = tks_to_cmd_tab(mms, node->tokens);
	if (!cmd_tab)
	{
		print_error("memory allocation failed"); // CHANGE
		return (1);
	}
	execve(exec->cmd_path, cmd_tab, NULL);
	perror("minishell");
	// CHANGE: 126 vs 127 were never distinguished (always returned 127).
	// errno tells us EACCES (found but not executable) from anything else.
	if (errno == EACCES)
		return (126);
	return (127);
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
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		perror("minishell"); // CHANGE
		return (1);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("minishell"); // CHANGE
		return (1);
	}
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
	status = redirection(mms, cmd);
	if (status != 0)
		return (status);
	//expand(mms, cmd->tokens); //error expander
	//if (builtin(cmd))
		//return (exec_builtin(mms, cmd));
	if (path_relative(cmd))
		exec.cmd_path = cmd->tokens[0]->value;
	else
	{
		exec.cmd_path = find_path(mms, cmd, &exec);
		if (exec.cmd_path == NULL)
		{
			print_cmd_not_found(cmd->tokens[0]->value);
			return (127);
		}
	}
	return (execute(mms, cmd, &exec));
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
	int	status;
	int	last_status;

	(void)node;
	i = 0;
	last_status = 0;
	while (pipeline->cmd_list[i + 1] != NULL)
	{
		if (pipe(pipeline->fd) == -1)
			return (1);
		pipeline->pids[i] = fork();
		if (pipeline->pids[i] == -1)
		{
			perror("minishell"); // CHANGE
			close(pipeline->fd[0]);
			close(pipeline->fd[1]);
			return (1);
		}
		if (pipeline->pids[i] == 0)
		{
			close(pipeline->fd[0]); // CHANGE: l'enfant n'écrit que sur ce pipe, il n'a pas besoin du bout lecture
			if (i == 0)
				exit(execute_cmd_pipe(mms,
				pipeline->cmd_list[i],
				STDIN_FILENO,
				pipeline->fd[1]));
			else
				exit(execute_cmd_pipe(mms,
				pipeline->cmd_list[i],
				pipeline->fd_prev,
				pipeline->fd[1]));
		}
		close(pipeline->fd[1]);
		if (pipeline->fd_prev != -1)
			close(pipeline->fd_prev);
		pipeline->fd_prev = pipeline->fd[0];
		i++;
	}
	pipeline->pids[i] = fork();
	if (pipeline->pids[i] == -1)
	{
		perror("minishell"); // CHANGE
		return (1);
	}
	if (pipeline->pids[i] == 0)
		exit(execute_cmd_pipe(mms,
				pipeline->cmd_list[i],
				pipeline->fd_prev,
				STDOUT_FILENO));
	if (pipeline->fd_prev != -1)
		close(pipeline->fd_prev);
	i = 0;
	while (i < pipeline->nb_cmd)
	{
		// CHANGE: waitpid() return value is now checked
		if (waitpid(pipeline->pids[i], &status, 0) == -1)
		{
			perror("minishell"); // CHANGE
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
 * @brief Prepares and executes a pipeline of commands.
 *
 * Counts the number of commands in the pipeline, allocates the array of AST
 * command nodes and the array of process identifiers, then extracts each
 * command from the pipeline before delegating their execution.
 *
 * @param mms  Pointer to the main minishell structure.
 * @param node AST node representing the root of the pipeline.
 *
 * @return The exit status returned by the pipeline execution.
 */

int	pipeline(t_mms *mms, t_ast *node)
{
	t_pipeline	*pipeline;

	pipeline = stack_alloc(mms->sa, sizeof(t_pipeline));
	if (!pipeline)
	{
		print_error("memory allocation failed"); // CHANGE
		return (1);
	}
	pipeline->nb_cmd = count_cmd_pipeline(node);
	pipeline->cmd_list = add_cmd_pipeline(mms, node,
			pipeline->nb_cmd);
	if (!pipeline->cmd_list)
	{
		print_error("memory allocation failed"); // CHANGE
		return (1);
	}
	pipeline->pids = stack_alloc(mms->sa,
			sizeof(pid_t) * pipeline->nb_cmd);
	if (!pipeline->pids)
	{
		print_error("memory allocation failed"); // CHANGE
		return (1);
	}
	pipeline->fd_prev = -1;
	return (execute_pipeline(mms, node, pipeline));
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
	//int			saved_in;
	//int			saved_out;

	// CHANGE: builtins used to always be forked, so cd/export/unset/exit
	// never had any effect on the parent shell. They now run here directly.
	/*if (builtin(node))
	{
		saved_in = dup(STDIN_FILENO);
		saved_out = dup(STDOUT_FILENO);
		status = redirection(mms, node);
		if (status == 0)
		{
			expand(mms, node->tokens); //error expand
			status = exec_builtin(mms, node);
		}
		dup2(saved_in, STDIN_FILENO); // restore the shell's own stdin
		dup2(saved_out, STDOUT_FILENO); // restore the shell's own stdout
		close(saved_in);
		close(saved_out);
		return (status);
	}*/
	exec.pid = fork();
	if (exec.pid == -1)
	{
		perror("minishell"); // CHANGE
		return (1);
	}
	if (exec.pid == 0)
	{
		signal(SIGPIPE, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		status = redirection(mms, node);
		if (status != 0)
			exit(status);
		//expand(mms, node->tokens); //error expand
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
	if (waitpid(exec.pid, &status, 0) == -1) // CHANGE: retour vérifié + message
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
 * @brief Executes an AST node according to its type.
 *
 * Dispatches the execution of a command, pipeline, logical AND, or logical
 * OR node. Logical operators are evaluated conditionally based on the exit
 * status of the left-hand side expression.
 *
 * @param mms  Pointer to the main minishell structure.
 * @param head Root node of the AST subtree to execute.
 *
 * @return The exit status of the executed command or expression.
 */

int	executor(t_mms *mms, t_ast *head)
{
	t_ast	*tmp;
	int		exit_status;

	if (!head)
		return (0);
	tmp = head;
	exit_status = 0;
	if (tmp->type == NODE_CMD)
		exit_status = execute_cmd(mms, tmp);
	else if (tmp->type == NODE_AND)
	{
		exit_status = executor(mms, tmp->left);
		if (exit_status == 0)
			exit_status = executor(mms, tmp->right);
	}
	else if (tmp->type == NODE_OR)
	{
		exit_status = executor(mms, tmp->left);
		if (exit_status != 0)
			exit_status = executor(mms, tmp->right);
	}
	else if (tmp->type == NODE_PIPE)
		exit_status = pipeline(mms, tmp);
	mms->last_status = exit_status;
	return (exit_status);
}