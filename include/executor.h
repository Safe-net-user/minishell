/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/11 11:23:14 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/18 10:39:06 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

typedef struct s_executor
{
    pid_t pid;
    int fd_in;
    int fd_out;
    char *cmd_path;
} t_executor;

typedef struct s_pipeline
{
    int nb_cmd;
    int fd_prev;
    int fd_next;
    int fd[2];
    t_ast **cmd_list;
    pid_t *pids;
} t_pipeline;

# include <unistd.h>
# include "minishell.h"
# include "parser.h"
# include "lexer.h"

int     executor(t_mms *mms, t_ast *head);

#endif