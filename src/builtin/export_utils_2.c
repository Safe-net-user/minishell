/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:49:07 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/24 23:50:41 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include <stdio.h>
#include "minishell.h"
#include "ft_strings.h"
#include <unistd.h>
#include "ft_io.h"

int	print_exported_var(t_env *env)
{
	t_env_entry	*arr;
	t_env_entry	*curr;

	arr = malloc(sizeof(*arr) * (env->entries + 1));
	if (!arr)
		return (ft_putstr_fd("miniMishell: export: \
internal error\n", STDERR_FILENO), 1);
	ht_to_arr(env, arr);
	if (!sort_arr(arr))
	{
		free(arr);
		ft_putstr_fd("miniMishell: export: internal error\n", STDERR_FILENO);
		return (1);
	}
	curr = arr;
	while (curr->key)
	{
		if (curr->value)
			printf("export %s=\"%s\"\n", curr->key, curr->value);
		else
			printf("export %s\n", curr->key);
		curr++;
	}
	free(arr);
	return (0);
}

int	alone_key_job(t_mms *mms, char *buffer_key)
{
	t_env_entry	*entry;

	entry = get_env(mms->env, buffer_key);
	if (!entry)
		return (!add_env(mms->env, buffer_key, NULL, EXPORTED));
	entry->flags |= EXPORTED;
	return (0);
}

t_builts_val	copy_key(char *dst, char *src, size_t *i)
{
	while (src[*i] && src[*i] != '=')
	{
		if (*i >= ENV_KEY_MAX)
		{
			ft_putstr_fd("miniMishell: export: key is \
too long\n", STDERR_FILENO);
			return (1);
		}
		dst[*i] = src[*i];
		(*i)++;
	}
	dst[*i] = '\0';
	if (!is_validname(dst))
	{
		ft_putstr_fd("miniMishell: export: invalid \
identifier\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

t_builts_val	copy_value(char *dst, char *src, size_t *i)
{
	size_t	j;

	j = 0;
	while (src[*i])
	{
		if (j >= ENV_NAME_MAX)
		{
			ft_putstr_fd("miniMishell: export: value is \
too long\n", STDERR_FILENO);
			return (1);
		}
		dst[j++] = src[(*i)++];
	}
	dst[j] = '\0';
	return (0);
}
