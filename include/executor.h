/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/11 11:23:14 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 19:17:39 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "minishell.h"
# include "parser.h"

typedef struct s_executor
{
	char	*cmd_path;
	pid_t	pid;
	int		fd_in;
	int		fd_out;
	char	padding[4];
}	t_executor;

typedef struct s_pipeline
{
	t_ast	**cmd_list;
	pid_t	*pids;
	int		nb_cmd;
	int		fd_prev;
	int		fd_next;
	int		fd[2];
	char	padding[4];
}	t_pipeline;

typedef struct s_saved_fd
{
	int	in;
	int	out;
}	t_saved_fd;

/* Executor */

int		executor(t_mms *mms, t_ast *head);
int		save_std_fd(t_saved_fd *saved);
void	restore_std_fd(t_saved_fd *saved);
int		builtin(t_ast *node);
int		exec_builtin(t_mms *mms, t_ast *node);
int		execute_cmd(t_mms *mms, t_ast *node);
int		exec_command(char *cmd_path, char **cmd_tab, char **envp);
int		execute(t_mms *mms, t_ast *node, t_executor *exec);
void	execute_child(t_mms *mms, t_ast *node);
int		fork_and_run(t_mms *mms, t_ast *node);

/* Pipeline */

int		execute_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out);
int		execute_pipeline(t_mms *mms, t_ast *node, t_pipeline *pipeline);
int		fork_pipeline_stage(t_mms *mms, t_pipeline *pipeline, int i,
			int old_fd_prev);
int		wait_child(pid_t pid);
void	pipeline_child(t_mms *mms, t_pipeline *pipeline, int i);
int		pipeline(t_mms *mms, t_ast *node);
int		pipeline_error(t_mms *mms);
void	restore_interactive_state(t_mms *mms);

/* Command */

char	**tks_to_cmd_tab(t_mms *mms, t_tk *tokens);
char	**hash_table_to_envp(t_ht *ht);

/* Errors */

void	print_error(char *msg);
void	print_cmd_not_found(char *cmd);
void	print_exec_error(char *path, char *msg);
void	print_signal_msg(int status);

/* Path */

char	*find_path(t_mms *mms, t_ast *node, t_executor *exec);
int		path_relative(t_ast *node);
char	*get_cmd_path_child(t_mms *mms, t_ast *node, t_executor *exec);
char	**get_search_path(t_mms *mms);

/* Redirection */

int		execute_redir_only(t_mms *mms, t_ast *node);
int		redirection(t_mms *mms, t_ast *node);

#endif
