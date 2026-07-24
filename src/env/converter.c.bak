/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   converter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 14:01:35 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 15:02:50 by fiaudfiz         ###   ########.fr       */
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
		free(start); // CHANGE: add_env copie key/value en interne (ft_strdup), donc start doit être libéré ici
		i++;
	}
	return (1);
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
	char	**envp;
	size_t	i;
	size_t	j;
	char	*line;

	envp = malloc(sizeof(char *) * (env->entries + 1));
	if (!envp)
		return (NULL);
	i = 0;
	j = 0;
	while (i < env->capacity)
	{
		if (env->indexes[i].key != NULL && env->indexes[i].key != DELETED
			&& (env->indexes[i].flags & EXPORTED))
		{
			line = ft_strjoin(env->indexes[i].key, "=");
			if (!line)
				return (free(envp), NULL);
			envp[j] = ft_strjoin(line, env->indexes[i].value);
			free(line);
			if (!envp[j])
				return (free(envp), NULL);
			j++;
		}
		i++;
	}
	envp[j] = NULL;
	return (envp);
}