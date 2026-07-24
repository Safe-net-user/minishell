/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:42:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/13 16:26:10 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include <stdio.h>
#include "minishell.h"
#include "ft_strings.h"
#include <unistd.h>
#include "ft_io.h"

static int	print_exported_var(t_env *env)
{
	t_env_entry	*arr;
	t_env_entry	*curr;

	arr = malloc(sizeof(*arr) * (env->entries + 1));
	if (!arr)
		return (ft_putstr_fd("miniMishell: export: internal error\n", STDERR_FILENO), 1);
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

static int	alone_key_job(t_mms *mms, char *buffer_key)
{
	t_env_entry	*entry;

	entry = get_env(mms->env, buffer_key);
	if (!entry)
		return (!add_env(mms->env, buffer_key, NULL, EXPORTED));
	entry->flags |= EXPORTED;
	return (0);
}

static t_builts_val	copy_key(char *dst, char *src, size_t *i)
{
	while (src[*i] && src[*i] != '=')
	{
		if (*i >= ENV_KEY_MAX)
		{
			ft_putstr_fd("miniMishell: export: key is too long\n", STDERR_FILENO);
			return (1);
		}
		dst[*i] = src[*i];
		(*i)++;
	}
	dst[*i] = '\0';
	if (!is_validname(dst))
	{
		ft_putstr_fd("miniMishell: export: invalid identifier\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

static t_builts_val	copy_value(char *dst, char *src, size_t *i)
{
	size_t	j;

	j = 0;
	while (src[*i])
	{
		if (j >= ENV_NAME_MAX)
		{
			ft_putstr_fd("miniMishell: export: value is too long\n", STDERR_FILENO);
			return (1);
		}
		dst[j++] = src[(*i)++];
	}
	dst[j] = '\0';
	return (0);
}

static t_builts_val	assign(t_mms *mms, char *buffer_key, char *src)
{
	char	buffer_value[ENV_NAME_MAX];
	size_t	i;

	i = 0;
	if (copy_key(buffer_key, src, &i))
		return (1);
	if (!src[i])
		return (alone_key_job(mms, buffer_key));
	i++;
	if (copy_value(buffer_value, src, &i))
		return (1);
	if (!add_env(mms->env, buffer_key, buffer_value, EXPORTED))
		return (1);
	return (0);
}

static t_builts_val	update_path_env(t_mms *mms, char *key)
{
	if (ft_strcmp(key, "PATH"))
		return (0);
	free_hash_table(mms->cmd_path);
	mms->cmd_path = init_hash_table(INIT_SIZE_HT);
	if (!mms->cmd_path)
		return (1);
	return (0);
}

static t_builts_val	parse_export_arg(t_mms *mms, char *src)
{
	t_builts_val	ret;
	char			buffer_key[ENV_KEY_MAX];

	ret = assign(mms, buffer_key, src);
	if (ret)
		return (ret);
	return (update_path_env(mms, buffer_key));
}

static t_builts_val	export_loop(t_mms *mms, char **arg)
{
	int				i;
	t_builts_val	ret;

	i = 1;
	while (arg[i])
	{
		ret = parse_export_arg(mms, arg[i]);
		if (ret)
			return (ret);
		i++;
	}
	return (0);
}

t_builts_val	builtin_export(t_mms *mms, char **arg)
{
	if (!mms || !mms->env || !arg)
	{
		ft_putstr_fd("miniMishell: export: internal error\n", STDERR_FILENO);
		return (1);
	}
	if (!arg[1])
		return (print_exported_var(mms->env));
	return (export_loop(mms, arg));
}
