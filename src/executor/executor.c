/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:20:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/11 17:29:54 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/*
* fork
* activate redirection (dup2)
* expand
* builtins
* found path or direct path
* exceve
*/

int execute_pipe(t_mms *mms, t_ast *node)
{
    
}

/*
* fork 
* redirections
* expand
* builtins
* path
* exceve
*/

int execute_cmd(t_mms *mms, t_ast *node)
{
    t_executor *exec;
    
    exec->pid = fork();
	if (exec->pid == -1)
		return(1); //err in fork
    if (exec->pid == 0)
    {
        if (redirection(mms, node != 0))
            return (redirection(mms, node));
        if (/*flag expand active*/)
            expand(mms, node);
    }
}

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
        signal = execute_pipe(mms, tmp);
    return (signal);
}