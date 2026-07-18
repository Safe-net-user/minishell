/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 17:20:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/18 10:24:56 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "ft_strings.h"

char *find_path(t_mms *mms, t_ast *node, t_executor *exec)
{
    char    **path;
    char    *temp;
    int     i;

    i = 0;
    path = ft_split(get_pointer(mms->cmd_path, "PATH"), ':');
    if (!path || !path[0])
        return (NULL);
    while (path[i])
    {
        temp = ft_strjoin(path[i], "/");
		if (!temp)
			return (NULL);
		temp = ft_strjoin_free(temp, node->argv[0], 1);
		if (!temp)
			return (NULL);
		if (access(temp, F_OK | X_OK) == 0)
			return (temp);
		free(temp);
		i++;
    }
    return (NULL);
}

int path_relative(t_ast *node)
{
    int i;

    i = 0;
    while(node->argv[0][i])
    {
        if (node->argv[0][i] == '/')
            return(1);
        i++;
    }
    return(0);
}

/*
on est deja dans un fork on doit trouvre les args de la fonction execute
*/

int execute(t_mms *mms, t_ast *node, t_executor *exec)
{
    if(exec->cmd_path != NULL)
        execve(node->argv[0], node->argv, NULL); //re-convertir hash table en char **
    else
        execve(exec->cmd_path, node->argv, NULL); //error = exceve 127 si == -1 //64
    return (0);
}
















































































int execute_cmd_pipe(t_mms *mms, t_ast *cmd, int fd_in, int fd_out)
{
    t_executor *exec;
    
    dup2(fd_in, STDIN_FILENO);
    dup2(fd_out, STDOUT_FILENO);
    close(fd_in);
    close(fd_out);
	if (redirection(mms, cmd) != 0)
        return (redirection(mms, cmd));
    if (/*flag expand active*/)
        expand(mms, cmd);
    if (builtin(cmd))
        return (exec_builtin(mms, cmd));
    if (path_relative(cmd) == 1)
        return (execute(mms, cmd, exec));
    else
    {
        exec->cmd_path = find_path(mms, cmd, exec); //return NULL = command not found 127
        return (execute(mms, cmd, exec));
    }
}

int execute_pipeline(t_mms *mms, t_ast *node, t_pipeline *pipeline)
{
    int i;

    i = 0;
    while (pipeline->cmd_list[i + 1] != NULL) //avant dernier car dernier pas de pipe
    {
        if (pipe(pipeline->fd) == -1)
            return (1);
        pipeline->pids[i] = fork();
        if (pipeline->pids[i] == -1)
            return (1);
        if (pipeline->pids[i] == 0)
        {
            if (i == 0)
                execute_cmd_pipe(mms, pipeline->cmd_list[i], STDIN_FILENO, pipeline->fd[1]);
            else
                execute_cmd_pipe(mms, pipeline->cmd_list[i], pipeline->fd_prev, pipeline->fd[1]);
        }
        close(pipeline->fd[1]);
        if (pipeline->fd_prev != -1)
            close(pipeline->fd_prev);
        pipeline->fd_prev = pipeline->fd[0];
        i++;
    }
    execute_cmd_pipe(mms, pipeline->cmd_list[i], pipeline->fd_prev, STDOUT_FILENO);
    return (1);
}

/*
* executer tous les pipes d'affilees: 
* fork
* activate redirection de pipe
* activate redirection command
* expand
* builtins
* found path or direct path
* exceve
*/

int pipeline(t_mms *mms, t_ast *node)
{
    t_pipeline *pipeline;
    t_ast *temp;
    int i;
    
    i = 0;
    temp = node;
    pipeline->nb_cmd = count_cmd_pipeline(node);
    pipeline->cmd_list = stack_alloc(mms->sa, sizeof(t_ast * ) * pipeline->nb_cmd + 1);
    pipeline->pids = stack_alloc(mms->sa, sizeof(pid_t) * pipeline->nb_cmd);
    temp = node;
    while (temp->left == NODE_PIPE)
    {
        pipeline->cmd_list[i] = add_cmd_pipeline(temp, pipeline);
        i++;
        temp = temp->left;
    }
    pipeline->cmd_list[i] = NULL;
    return (execute_pipeline(mms, node, pipeline));
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
		return(1); //err in fork 1
    if (exec->pid == 0)
    {
        if (redirection(mms, node) != 0)
            return (redirection(mms, node));
        if (/*flag expand active*/)
            expand(mms, node);
        if (builtin(node))
            return (exec_builtin(mms, node));
        if (path_relative(node) == 1)
            return (execute(mms, node, exec));
        else
        {
            exec->cmd_path = find_path(mms, node, exec); //return NULL = command not found 127
            return (execute(mms, node, exec));
        }
    }
    //voir on return quoi
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
        signal = pipeline(mms, tmp);
    return (signal);
}
