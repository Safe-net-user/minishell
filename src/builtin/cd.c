/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:07:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/18 13:49:19 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "env.h"
#include "ft_strings.h"
#include "ft_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static int	update_entry(t_env *env, char *key, char *value)
{
	t_env_entry	*entry;

	entry = get_env(env, key);
	if (entry)
	{
		free(entry->value);
		entry->value = value;
	}
	else
	{
		add_env(env, key, value, EXPORTED);
		free(value);
	}
	return (BUI_SUCCESS);
}

static int	update_pwd(t_env *env, char *old_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		return (BUI_ERROR);
	update_entry(env, "OLDPWD", old_pwd);
	update_entry(env, "PWD", new_pwd);
	return (BUI_SUCCESS);
}

static char	*get_cd_target(t_env *env, char **argv, char *old_pwd)
{
	t_env_entry	*entry;

	if (!argv[1] || !argv[1][0])
		entry = get_env(env, "HOME");
	else if (!ft_strcmp(argv[1], "-"))
		entry = get_env(env, "OLDPWD");
	else
		return (argv[1]);
	if (!entry || !entry->value)
	{
		free(old_pwd);
		ft_putstr_fd("miniMishell: cd: variable not set\n", STDERR_FILENO);
		return (NULL);
	}
	return (entry->value);
}

static int	do_cd(t_env *env, char *path, char *old_pwd)
{
	if (chdir(path))
	{
		free(old_pwd);
		perror("miniMishell: cd");
		return (BUI_ERROR);
	}
	return (update_pwd(env, old_pwd));
}

t_builts_val	builtin_cd(t_env *env, char **argv)
{
	char	*old_pwd;
	char	*path;

	if (!env || !argv)
		return (BUI_ERROR);
	if (argv[1] && argv[2])
		return (ft_putstr_fd("miniMishell: cd: too many arguments\n", \
STDERR_FILENO), BUI_TOO_MANY_ARGS);
	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		return (perror("miniMishell: cd"), BUI_ERROR);
	path = get_cd_target(env, argv, old_pwd);
	if (!path)
		return (BUI_ERROR);
	return (do_cd(env, path, old_pwd));
}
