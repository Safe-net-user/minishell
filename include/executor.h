/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/11 11:23:14 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 14:20:25 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "minishell.h"
# include "parser.h"

typedef struct s_executor
{
	pid_t	pid;
	int		fd_in;
	int		fd_out;
	char	*cmd_path;
}	t_executor;

typedef struct s_pipeline
{
	int		nb_cmd;
	int		fd_prev;
	int		fd_next;
	int		fd[2];
	t_ast	**cmd_list;
	pid_t	*pids;
}	t_pipeline;

typedef struct s_saved_fd
{
	int	in;
	int	out;
}	t_saved_fd;

int		executor(t_mms *mms, t_ast *head);
int	save_std_fd(t_saved_fd *saved);
void	restore_std_fd(t_saved_fd *saved);

int		builtin(t_ast *node);
int		exec_builtin(t_mms *mms, t_ast *node);

int		execute_cmd(t_mms *mms, t_ast *node);
int		execute_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out);
char	**tks_to_cmd_tab(t_mms *mms, t_tk *tokens);
int		execute(t_mms *mms, t_ast *node, t_executor *exec);

void	print_error(char *msg);
void	print_cmd_not_found(char *cmd);
void	print_exec_error(char *path, char *msg);

char	*here_doc(t_mms *mms, t_tk *redir);
int write_heredoc_content(int fd[2], char *content);

char	*find_path(t_mms *mms, t_ast *node, t_executor *exec);
int		path_relative(t_ast *node);
char 	*get_cmd_path_child(t_mms *mms, t_ast *node, t_executor *exec);

int		pipeline(t_mms *mms, t_ast *node);
int		execute_pipeline(t_mms *mms, t_ast *node, t_pipeline *pipeline);

int		redirection(t_mms *mms, t_ast *node);

char	**hash_table_to_envp(t_ht *ht);

int		execute_redir_only(t_mms *mms, t_ast *node);

int		exec_command(char *cmd_path, char **cmd_tab, char **envp);

#endif
