/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils_1.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 16:24:40 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 16:28:34 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static void	free_envp(char **envp, size_t size)
{
	while (size > 0)
		free(envp[--size]);
	free(envp);
}

static char	*entry_to_envp(t_ht_entry *entry)
{
	char	*env;
	size_t	len;

	len = ft_strlen(entry->key) + ft_strlen(entry->value) + 2;
	env = malloc(len);
	if (!env)
		return (NULL);
	ft_strlcpy(env, entry->key, len);
	ft_strlcat(env, "=", len);
	ft_strlcat(env, entry->value, len);
	return (env);
}

static int	add_envp_entry(char **envp, size_t *j, t_ht_entry *entry)
{
	if (!entry->key || entry->key == (char *)DELETED)
		return (0);
	envp[*j] = entry_to_envp(entry);
	if (!envp[*j])
		return (-1);
	(*j)++;
	return (0);
}

char	**hash_table_to_envp(t_ht *ht)
{
	char	**envp;
	size_t	i;
	size_t	j;

	envp = malloc(sizeof(char *) * (ht->entries + 1));
	if (!envp)
		return (NULL);
	i = 0;
	j = 0;
	while (i < ht->capacity)
	{
		if (add_envp_entry(envp, &j, &ht->indexes[i]) == -1)
		{
			free_envp(envp, j);
			return (NULL);
		}
		i++;
	}
	envp[j] = NULL;
	return (envp);
}
