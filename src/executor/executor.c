/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:20:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 00:54:55 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "ft_strings.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

/*juste pour <*/

int redirection_in(t_mms *mms, t_redir *redir)
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

/*pour > ou >>*/

int redirection_out(t_mms *mms, t_redir *redir)
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

/*
* ici on recoit les redirections sous la forme d'une liste chainee de in et de out
* elles sont dans l'ordre ou elles ont ete rentres
* ouvrir le fd pour voir si pb, si un pb tout arreter
* si non pb activer avec dup 2
* aller a la suivante
*/

int redirection(t_mms *mms, t_ast *node)
{
	t_redir	*redir;
	int		status;

	redir = node->redirect;
	while (redir != NULL)
	{
		if (redir->type_tk == TOK_DLESS)
			status = heredoc(mms, node);
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

/*
* renvoyer vers les fonctions builtins, normalement le fork est deja fait
* il fauudra renvoyer a chaque fois aussi les codes d'erreur
* update aussi la variable globale
* 4. Builtins dans execute_cmd — fork inutile — cd, export, unset, exit doivent tourner dans le processus parent, pas dans un enfant. 
* Si tu forkes pour cd, le chdir n'affecte pas le shell.
*/

int exec_builtin(t_mms *mms, t_ast *node)
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
}

/*
* juste rehcercher un builtin donc return 1 ou 0
*/

int builtin(t_ast *node)
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
}

/*
* compter le nombre de pipe plus 2
*/

int	count_cmd_pipeline(t_ast *node)
{
	if (node->type != NODE_PIPE)
		return (1);
	return (count_cmd_pipeline(node->left) + 1);
}

/*fonction*/
static void	fill_pipeline_cmds(t_ast *node, t_ast **cmd_list, int *i)
{
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

/*
* convertir un pipeline en tableau de commandes
* il faut dans l'ordre donc on commence en bas a gauche puis on remonte
* sauf que on a pas d'outil pour remonter donc peut etre remplir par la fin ?
* fonction a faire en reucrsion je pense
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

char *find_path(t_mms *mms, t_ast *node, t_executor *exec)
{
    char    **path;
    char    *temp;
    int     i;

    i = 0;
    path = ft_split(get_pointer(mms->cmd_path, "PATH"), ':'); //malloc a free
    if (!path || !path[0])
        return (NULL);
    while (path[i])
    {
        temp = ft_strjoin(path[i], "/");
		if (!temp)
			return (NULL);
		temp = ft_strjoin_free(temp, node->tokens[0]->value, 1);
		if (!temp)
			return (NULL);
		if (access(temp, F_OK | X_OK) == 0)
			return (temp);
		free(temp);
		i++;
    }
    return (NULL);
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

int path_relative(t_ast *node)
{
    if (ft_strchr(node->tokens[0]->value, '/'))
        return (1);
    return (0);
}

/**
 * @brief Executes an external command using execve().
 *
 * Executes the command using either its directly specified path or the path
 * resolved by the command lookup mechanism. If execve() fails, the function
 * returns to the caller.
 *
 * @param mms  Pointer to the main minishell structure.
 * @param node AST node representing the command to execute.
 * @param exec Execution context containing the resolved command path.
 *
 * @return 0 if execve() returns to the caller after a failure.
 */

int execute(t_mms *mms, t_ast *node, t_executor *exec)
{
	(void)mms;
	if (execve(exec->cmd_path, /*char ** a construire*/, NULL) == -1)
	{
		perror("minishell");
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
 * @param mms    Pointer to the main minishell structure.
 * @param cmd    AST node representing the command to execute.
 * @param fd_in  File descriptor used as the command's standard input.
 * @param fd_out File descriptor used as the command's standard output.
 *
 * @return The exit status of the executed builtin or external command, or a
 *         non-zero status if a redirection fails.
 */

int execute_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out)
{
	t_executor	exec;
	int			status;

	if (dup2(fd_in, STDIN_FILENO) == -1)
		return (1);
	if (dup2(fd_out, STDOUT_FILENO) == -1)
		return (1);
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
	status = redirection(mms, cmd);
	if (status != 0)
		return (status);
	expand(mms, cmd->tokens);
	if (builtin(cmd))
		return (exec_builtin(mms, cmd));
	if (path_relative(cmd))
		exec.cmd_path = cmd->tokens[0]->value;
	else
	{
		exec.cmd_path = find_path(mms, cmd, &exec);
		if (exec.cmd_path == NULL)
			return (127);
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
 * @return 1 if a pipe or fork operation fails, or after the pipeline execution
 *         has been initiated.
 */

int execute_pipeline(t_mms *mms, t_ast *node, t_pipeline *pipeline)
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
			return (1);
		if (pipeline->pids[i] == 0)
		{
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
		return (1);
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
		waitpid(pipeline->pids[i], &status, 0);
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
		return (1);
	pipeline->nb_cmd = count_cmd_pipeline(node);
	pipeline->cmd_list = add_cmd_pipeline(mms, node,
			pipeline->nb_cmd);
	if (!pipeline->cmd_list)
		return (1);
	pipeline->pids = stack_alloc(mms->sa,
			sizeof(pid_t) * pipeline->nb_cmd);
	if (!pipeline->pids)
		return (1);
	pipeline->fd_prev = -1;
	return (execute_pipeline(mms, node, pipeline));
}

/**
 * @brief Executes a single command in a child process.
 *
 * Creates a child process to execute the command, applies its redirections,
 * performs expansion when required, executes builtins directly, and resolves
 * the executable path before calling execve().
 *
 * @param mms  Pointer to the main minishell structure.
 * @param node AST node representing the command to execute.
 *
 * @return The status of the fork operation, or the command execution status
 *         in the child process.
 */

int execute_cmd(t_mms *mms, t_ast *node)
{
	t_executor	exec;
	int			status;

	exec.pid = fork();
	if (exec.pid == -1)
		return (1);
	if (exec.pid == 0)
	{
		status = redirection(mms, node);
		if (status != 0)
			exit(status);
		expand(mms, node->tokens);
		if (builtin(node))
			exit(exec_builtin(mms, node));
		if (path_relative(node))
			exec.cmd_path = node->tokens[0]->value;
		else
		{
			exec.cmd_path = find_path(mms, node, &exec);
			if (exec.cmd_path == NULL)
				exit(127);
		}
		exit(execute(mms, node, &exec));
	}
	waitpid(exec.pid, &status, 0);
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

int executor(t_mms *mms, t_ast *head)
{
    t_ast *tmp;
    int signal = 0;

    tmp = head;
    if (tmp->type == NODE_CMD)
        signal = execute_cmd(mms, tmp);
    else if (tmp->type == NODE_AND)
    {
        signal = executor(mms, tmp->left);
        if (signal == 0)
            signal = executor(mms, tmp->right);
    }
    else if (tmp->type == NODE_OR)
    {
        signal = executor(mms, tmp->left);
        if (signal != 0)
            signal = executor(mms, tmp->right);
    }
    else if (tmp->type == NODE_PIPE)
        signal = pipeline(mms, tmp);
    return (signal);
}
