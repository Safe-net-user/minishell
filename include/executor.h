/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/11 11:23:14 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/11 17:53:51 by fiaudfiz         ###   ########.fr       */
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
    char **cmd_tab;
} t_executor;

typedef int				(*t_format)(t_mms *mms, t_ast *node);


typedef struct format_stuct
{
	char		letter;
	t_format	format_alias;
}	t_alias;

extern const t_alias	g_format_tab[];
# include <unistd.h>
# include "minishell.h"
# include "parser.h"
# include "lexer.h"

int     executor(t_mms *mms, t_ast *head);

#endif