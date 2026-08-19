/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:28:26 by gaspard           #+#    #+#             */
/*   Updated: 2026/08/18 14:42:16 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include <stddef.h>
#include "env.h"
#include "ft_strings.h"
#include <stdio.h>

int	is_validname(char *str)
{
	size_t	i;
	char	c;

	i = 1;
	if (!str)
		return (0);
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	while (str[i] && str[i] != '=')
	{
		c = str[i];
		if (ft_isalpha(c) || c == '_' || ft_isdigit(c))
			i++;
		else
			return (0);
	}
	return (1);
}

void	ht_to_arr(t_env *env, t_env_entry *arr)
{
	t_env_entry	*entry;
	t_env_entry	*limit;
	t_env_entry	*curr_p;

	entry = env->indexes;
	limit = env->indexes + env->capacity;
	curr_p = arr;
	while (entry < limit)
	{
		if (entry->key != NULL && entry->key \
!= ((void *)-1) && entry->flags & EXPORTED)
		{
			*curr_p = *entry;
			curr_p++;
		}
		entry++;
	}
	curr_p->key = NULL;
}

size_t	len_arr(t_env_entry *entries)
{
	size_t	count;

	count = 0;
	while (entries->key)
	{
		count++;
		entries++;
	}
	return (count);
}

int	sort_arr(t_env_entry *entries)
{
	size_t		i;
	size_t		j;
	t_env_entry	*limit;
	size_t		n;

	if (!entries)
		return (0);
	n = len_arr(entries);
	i = 0;
	limit = entries + n;
	while (entries + i < limit)
	{
		j = i + 1;
		while (entries + j < limit - 1)
		{
			if (ft_strcmp(entries[i].key, entries[j].key) > 0)
				swap_addr(&entries[i], &entries[j]);
			j++;
		}
		i++;
	}
	return (1);
}
