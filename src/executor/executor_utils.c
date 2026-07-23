/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:47:54 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 00:05:10 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/**
 * @brief Converts an array of token structures into an array of command strings.
 *
 * Allocates an array of strings using the stack allocator and stores the value
 * of each token in the corresponding position. The resulting array is
 * NULL-terminated and can be used as a command argument vector.
 *
 * @param[in] mms    Pointer to the minishell main structure.
 * @param[in] tokens NULL-terminated array of token pointers.
 *
 * @return A NULL-terminated array containing the values of the provided tokens.
 */

char	**tks_to_cmd_tab(t_mms *mms, t_tk **tokens)
{
	char	**cmd_tab;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (tokens[i])
		i++;
	cmd_tab = stack_alloc(mms->sa, sizeof(char *) * (i + 1));
	if (!cmd_tab)
		return (NULL);
	while (j < i)
	{
		cmd_tab[j] = tokens[j]->value;
		j++;
	}
	cmd_tab[j] = NULL;
	return (cmd_tab);
}

char	**hash_table_to_envp(t_ht *ht)
{
	char	**envp;
	size_t	i;
	size_t	j;
	size_t	len;

	envp = malloc(sizeof(char *) * (ht->entries + 1));
	if (!envp)
		return (NULL);
	i = 0;
	j = 0;
	while (i < ht->capacity)
	{
		if (ht->indexes[i].key
			&& ht->indexes[i].key != (char *)DELETED)
		{
			len = strlen(ht->indexes[i].key)
				+ 1 + strlen(ht->indexes[i].value) + 1;
			envp[j] = malloc(len);
			if (!envp[j])
				return (NULL);
			strcpy(envp[j], ht->indexes[i].key);
			strcat(envp[j], "=");
			strcat(envp[j], ht->indexes[i].value);
			j++;
		}
		i++;
	}
	envp[j] = NULL;
	return (envp);
}