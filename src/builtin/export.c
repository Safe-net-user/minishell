/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:42:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:05:23 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file export.c
 * @brief `export` builtin implementation.
 *
 * Called with no arguments, `builtin_export()` lists exported
 * variables (`print_exported_var()`, typically `declare -x` style).
 * Otherwise it processes each `KEY` or `KEY=value` argument through
 * `parse_export_arg()`: `assign()` splits the argument at `=`
 * (`copy_key`/`copy_value`), and a bare `KEY` with no `=` just marks
 * an existing or new variable as exported without changing its
 * value (`alone_key_job()`).
 *
 * `update_path_env()` invalidates the command-path cache whenever
 * `PATH` itself is exported/reassigned, so stale resolved paths from
 * `mms->cmd_path` don't survive a `PATH` change.
 */

#include "builtin.h"
#include <stdio.h>
#include "minishell.h"
#include "ft_strings.h"
#include <unistd.h>
#include "ft_io.h"

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
