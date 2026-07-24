/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/11 11:23:14 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 14:16:21 by fiaudfiz         ###   ########.fr       */
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

int	executor(t_mms *mms, t_ast *head);

int	builtin(t_ast *node);
int	exec_builtin(t_mms *mms, t_ast *node);


int	execute_cmd(t_mms *mms, t_ast *node);
int	execute_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out);
char	**tks_to_cmd_tab(t_mms *mms, t_tk **tokens);
int	execute(t_mms *mms, t_ast *node, t_executor *exec);

void	print_error(char *msg);
void	print_cmd_not_found(char *cmd);

int	here_doc(t_mms *mms, t_redir *redir);

char	*find_path(t_mms *mms, t_ast *node, t_executor *exec);
int	path_relative(t_ast *node);

int	pipeline(t_mms *mms, t_ast *node);
int	execute_pipeline(t_mms *mms, t_ast *node, t_pipeline *pipeline);

int	redirection(t_mms *mms, t_ast *node);

char	**hash_table_to_envp(t_ht *ht);

int	execute_redir_only(t_mms *mms, t_ast *node);

#endif