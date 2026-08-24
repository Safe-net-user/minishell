/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:07:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/23 18:42:24 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "minishell.h"
#include "env.h"
#include "ft_strings.h"
#include "ft_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static t_builts_val	change_dir(t_mms *mms, char *path)
{
	char	*old_pwd;
	char	*new_pwd;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		return (internal_error());
	if (chdir(path) < 0)
	{
		perror("miniMishell: cd");
		free(old_pwd);
		return (BUI_ERROR);
	}
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		free(old_pwd);
		return (internal_error());
	}
	add_env(mms->env, "OLDPWD", old_pwd, EXPORTED);
	add_env(mms->env, "PWD", new_pwd, EXPORTED);
	free(mms->cwd);
	mms->cwd = new_pwd;
	free(old_pwd);
	return (BUI_SUCCESS);
}

static t_builts_val	cd_home(t_mms *mms)
{
	t_env_entry	*entry;

	entry = get_env(mms->env, "HOME");
	if (!entry || !entry->value)
		return (variable_not_set_error());
	return (change_dir(mms, entry->value));
}

static t_builts_val	cd_oldpwd(t_mms *mms)
{
	t_env_entry		*entry;
	char			*oldpwd;
	t_builts_val	ret;

	entry = get_env(mms->env, "OLDPWD");
	if (!entry || !entry->value || !entry->value[0])
		return (variable_not_set_error());
	oldpwd = ft_strdup(entry->value);
	if (!oldpwd)
		return (internal_error());
	ret = change_dir(mms, oldpwd);
	if (ret == BUI_SUCCESS)
	{
		ft_putstr_fd(oldpwd, STDOUT_FILENO);
		ft_putstr_fd("\n", STDOUT_FILENO);
	}
	free(oldpwd);
	return (ret);
}

t_builts_val	cd(t_mms *mms, char *path)
{
	char	*old_pwd;
	char	*new_path;

	old_pwd = getcwd(NULL, 0);
	new_path = get_new_path(mms, old_pwd, path);
	if (!old_pwd)
	{
		if (chdir(new_path) < 0)
			add_env(mms->env, "OLDPWD", mms->cwd, EXPORTED);
		old_pwd = getcwd(NULL, 0);
		if (!old_pwd)
			perror("miniMishell: cd");
		return (BUI_ERROR);
	}
	if (chdir(path) < 0)
	{
		perror("miniMishell: cd");
		add_env(mms->env, "OLDPWD", mms->cwd, EXPORTED);
		return (BUI_ERROR);
	}
	add_env(mms->env, "OLDPWD", old_pwd, EXPORTED);
	free(mms->cwd);
	ft_strdup(mms->cwd = new_path);
	return (BUI_SUCCESS);
}

t_builts_val	builtin_cd(t_mms *mms, char **av)
{
	if (!mms || !mms->env || !av)
		return (internal_error());
	if (av[2])
		return (too_many_args_error());
	if (!av[1] || !av[1][0])
		return (cd_home(mms));
	if (ft_strcmp(av[1], "-") == 0)
		return (cd_oldpwd(mms));
	return (cd(mms, av[1]));
}
