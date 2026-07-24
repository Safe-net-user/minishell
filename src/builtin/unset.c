/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaspard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:38:42 by gaspard           #+#    #+#             */
/*   Updated: 2026/07/24 23:42:33 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "env.h"
#include "ft_strings.h"
#include "minishell.h"
#include "ft_io.h"
#include <unistd.h>

static int	reset_cmd_path(t_mms *mms)
{
	free_hash_table(mms->cmd_path);
	mms->cmd_path = init_hash_table(INIT_SIZE_HT);
	if (!mms->cmd_path)
	{
		ft_putstr_fd("miniMishell: unset: internal error\n", STDERR_FILENO);
		return (0);
	}
	return (1);
}

static int	return_bad_alloc_msg(void)
{
	ft_putstr_fd("miniMishell: unset: internal error\n", STDERR_FILENO);
	return (0);
}

t_builts_val	builtin_unset(t_mms *mms, char **argv)
{
	t_env_entry	*entry;
	size_t		i;

	if (!mms || !mms->env || !argv)
		return (return_bad_alloc());
	i = 1;
	while (argv[i])
	{
		entry = get_env(mms->env, argv[i]);
		if (entry && entry->value)
		{
			if (entry->flags & READONLY)
			{
				fprintf(stderr, "miniMishell: unset: %s: \
readonly variable\n", entry->value);
				return (1);
			}
			if (!ft_strcmp(entry->key, "PATH") && !reset_cmd_path(mms))
				return (0);
			del_env(mms->env, argv[i]);
		}
		i++;
	}
	return (0);
}
