/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   converter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 14:01:35 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 17:30:08 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "ft_strings.h"
#include <stdio.h>
#include <stdlib.h>

int	env_arr_to_ht(char **envp, t_env *ht)
{
	size_t	i;
	size_t	j;
	char	*start;

	i = 0;
	while (envp[i])
	{
		j = 0;
		start = ft_strdup(envp[i]);
		if (!start)
			return (0);
		while (start[j] != '=')
			j++;
		start[j] = '\0';
		j++;
		add_env(ht, start, &start[j], EXPORTED);
		free(start);
		i++;
	}
	return (1);
}

static void	free_envp(char **envp, size_t size)
{
	while (size > 0)
		free(envp[--size]);
	free(envp);
}

static char	*env_entry_to_str(t_env_entry *entry)
{
	char	*line;
	char	*result;

	line = ft_strjoin(entry->key, "=");
	if (!line)
		return (NULL);
	result = ft_strjoin(line, entry->value);
	free(line);
	return (result);
}

static int	is_exported(t_env_entry *entry)
{
	return (entry->key != NULL
		&& entry->key != DELETED
		&& (entry->flags & EXPORTED));
}

/**
 * @brief Converts the environment hashtable into a char** envp array.
 *
 * Only exported entries are included, skipping deleted slots.
 *
 * @param env Environment hashtable.
 * @return Newly allocated NULL-terminated array of "KEY=VALUE" strings,
 *         or NULL on allocation failure.
 */
char	**env_to_envp(t_env *env)
{
	char		**envp;
	size_t		i;
	size_t		j;

	envp = malloc(sizeof(char *) * (env->entries + 1));
	if (!envp)
		return (NULL);
	i = 0;
	j = 0;
	while (i < env->capacity)
	{
		if (is_exported(&env->indexes[i]))
		{
			envp[j] = env_entry_to_str(&env->indexes[i]);
			if (!envp[j])
				return (free_envp(envp, j), NULL);
			j++;
		}
		i++;
	}
	envp[j] = NULL;
	return (envp);
}
