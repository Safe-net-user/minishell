/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ht_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:51:41 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 10:52:47 by fiaudfiz         ###   ########.fr       */
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

static t_env_val	insert_env(t_env_entry *entry, t_env_add *data)
{
	entry->key = ft_strdup(data->key);
	entry->value = ft_strdup(data->str);
	if (!entry->key || !entry->value)
	{
		free(entry->key);
		free(entry->value);
		return (ENV_ERROR);
	}
	entry->flags = data->flags;
	return (ENV_SUCCESS);
}

static t_env_val	find_env_slot(t_env *env, t_env_add *data, size_t n)
{
	ssize_t	index;
	ssize_t	deleted;

	index = joaat_hash((unsigned char *)data->key, n)
		% env->capacity;
	deleted = -1;
	while (env->indexes[index].key != NULL)
	{
		if (env->indexes[index].key == ((void *)-1))
		{
			if (deleted == -1)
				deleted = index;
		}
		else if (ft_strncmp(env->indexes[index].key,
				data->key, n + 1) == 0)
			return (update_env(&env->indexes[index], data));
		index = (index + 1) % env->capacity;
	}
	if (deleted != -1)
		index = deleted;
	if (insert_env(&env->indexes[index], data) == ENV_ERROR)
		return (ENV_ERROR);
	env->entries++;
	return (ENV_SUCCESS);
}

t_env_val	add_env(t_env *env, char *key, char *str, t_env_flags flags)
{
	t_env_add	data;
	size_t		n;

	if (!env || !key || !str)
		return (ENV_ERROR);
	if (env->entries == env->capacity)
		return (ENV_FULL);
	data.key = key;
	data.str = str;
	data.flags = flags;
	n = ft_strlen(key);
	return (find_env_slot(env, &data, n));
}

t_env_entry	*get_env(t_env *env, char *key)
{
	size_t	n;
	size_t	index;
	size_t	tmp_index;

	if (!env || !key)
		return (NULL);
	n = ft_strlen(key);
	index = joaat_hash((unsigned char *)key, n) % env->capacity;
	tmp_index = (index - 1) % env->capacity;
	while (env->indexes[index].key != NULL)
	{
		if (env->indexes[index].key != ((void *)-1))
		{
			if (ft_strncmp(env->indexes[index].key, key, n + 1) == 0)
				return (&env->indexes[index]);
		}
		if (__builtin_expect(tmp_index == index, 0))
			return (NULL);
		index = (index + 1) % env->capacity;
	}
	return (NULL);
}

t_env_val	del_env(t_env *env, char *key)
{
	size_t	n;
	size_t	index;

	if (!env || !key)
		return (ENV_ERROR);
	if (env->entries == 0)
		return (ENV_EMPTY);
	n = ft_strlen(key);
	index = joaat_hash((unsigned char *)key, n) % env->capacity;
	while (env->indexes[index].key != NULL)
	{
		if (ft_memcmp(env->indexes[index].key, key, n + 1) == 0)
		{
			env->entries--;
			free(env->indexes[index].value);
			free(env->indexes[index].key);
			env->indexes[index].key = ((void *)-1);
			return (ENV_SUCCESS);
		}
		index = (index + 1) % env->capacity;
	}
	return (ENV_ERROR);
}
