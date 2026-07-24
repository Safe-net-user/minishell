/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 16:56:45 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 08:52:15 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"

/**
 * @brief Frees a NULL-terminated array of strings produced by ft_split.
 *
 * @param[in] arr NULL-terminated array of strings to free.
 */

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
	char	**path;
	char	*result;

	(void)exec;
	path = ft_split(get_env(mms->env, "PATH")->value, ':');
	if (!path || !path[0])
	{
		free_path_arr(path);
		return (NULL);
	}
	result = search_path(path, node->tokens[0]->value);
	free_path_arr(path);
	return (result);
}
