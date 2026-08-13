/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:35:18 by gaspard           #+#    #+#             */
/*   Updated: 2026/08/13 11:08:57 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include <stdio.h>
#include "ft_io.h"

static int	print_exported_var_env(t_env *env)
{
	t_env_entry	*entry;
	t_env_entry	*limit;

	entry = env->indexes;
	limit = env->indexes + env->capacity;
	while (entry < limit)
	{
		if (entry->key != NULL
			&& entry->key != ((void *)-1)
			&& (entry->flags & EXPORTED)
			&& entry->value != NULL)
		{
			ft_putstr_fd(entry->key, STDOUT_FILENO);
			ft_putstr_fd("=", STDOUT_FILENO);
			ft_putstr_fd(entry->value, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		entry++;
	}
	return (BUI_SUCCESS);
}

t_builts_val	builtin_env(t_env *env, char **args)
{
	if (!env || !args)
		return (0);
	if (args[1])
	{
		ft_putstr_fd("bash: env: too many arguments\n", STDERR_FILENO);
		return (2);
	}
	return (print_exported_var_env(env));
}
