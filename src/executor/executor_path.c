/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:56:45 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 10:49:55 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"



void	free_path_arr(char **arr)
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

int	path_relative(t_ast *node)
{
	if (ft_strchr(node->tokens->value, '/'))
		return (1);
	return (0);
}

static char	*build_path(char *dir, char *cmd)
{
	char	*temp;
	char	*path;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	path = ft_strjoin(temp, cmd);
	free(temp);
	return (path);
}

static char	*search_path(char **path, char *cmd)
{
	char	*temp;
	int		i;

	i = 0;
	while (path[i])
	{
		temp = build_path(path[i], cmd);
		if (!temp)
			return (NULL);
		if (access(temp, F_OK | X_OK) == 0)
			return (temp);
		free(temp);
		i++;
	}
	return (NULL);
}

char	*find_path(t_mms *mms, t_ast *node, t_executor *exec)
{
	char		**path;
	char		*result;
	t_env_entry	*entry;

	(void)exec;
	if (!node->tokens->value)
		return (NULL);
	entry = get_env(mms->env, "PATH");
	if (!entry)
		return (NULL);
	path = ft_split(entry->value, ':');
	if (!path || !path[0])
	{
		free_path_arr(path);
		return (NULL);
	}
	result = search_path(path, node->tokens->value);
	free_path_arr(path);
	return (result);
}
