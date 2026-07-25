/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 02:12:08 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/25 02:12:47 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include <stdlib.h>
#include "ft_strings.h"
#include "ft_crypto.h"
#include "ft_printf.h"
#include "ft_memory.h"
#include "ft_stdlib.h"
#include <stdio.h>

t_env_val	update_env(t_env_entry *entry, t_env_add *data)
{
	free(entry->value);
	entry->value = ft_strdup(data->str);
	if (!entry->value)
		return (ENV_ERROR);
	entry->flags = data->flags;
	return (ENV_SUCCESS);
}

int	add_env_entry(t_env *ht, char *start)
{
	size_t	j;

	j = 0;
	while (start[j] && start[j] != '=')
		j++;
	if (start[j] != '=')
	{
		free(start);
		return (1);
	}
	start[j] = '\0';
	j++;
	if (add_env(ht, start, &start[j], EXPORTED) == ENV_ERROR)
	{
		free(start);
		return (0);
	}
	free(start);
	return (1);
}

t_env	*init_env(size_t n)
{
	t_env	*env;

	if (n == 0 || n > SIZE_MAX / sizeof(t_env_entry))
		return (NULL);
	env = ft_calloc(1, sizeof(t_env));
	if (!env)
		return (NULL);
	env->indexes = ft_calloc(n, sizeof(t_env_entry));
	if (!env->indexes)
	{
		free(env);
		return (NULL);
	}
	env->entries = 0;
	env->capacity = n;
	return (env);
}

void	free_env(t_env *env)
{
	size_t	i;

	if (!env)
		return ;
	i = 0;
	while (i < env->capacity)
	{
		if (env->indexes[i].key != NULL && env->indexes[i].key != ((void *)-1))
		{
			free(env->indexes[i].value);
			free(env->indexes[i].key);
		}
		i++;
	}
	free(env->indexes);
	free(env);
}
