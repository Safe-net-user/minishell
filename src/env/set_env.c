/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:49:27 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/19 15:59:30 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include "ft_strings.h"
#include "ft_stdlib.h"
#include "env.h"
#include <unistd.h>

static int	init_underscore(t_env *env, char *path)
{
	if (!get_env(env, "_"))
	{
		if (add_env(env, "_", path, EXPORTED + READONLY) == ENV_ERROR)
			return (0);
	}
	return (1);
}

static int	init_shlvl(t_env *env)
{
	t_env_entry	*entry;
	char		*lvl_str;
	int			shell_lvl;

	entry = get_env(env, "SHLVL");
	if (!entry)
		return (add_env(env, "SHLVL", "1", EXPORTED) != ENV_ERROR);
	shell_lvl = ft_atoi(entry->value) + 1;
	lvl_str = ft_itoa(shell_lvl);
	if (!lvl_str)
		return (0);
	add_env(env, "SHLVL", lvl_str, EXPORTED);
	free(lvl_str);
	return (1);
}

static int	init_pwd(t_env *env)
{
	char	*pwd;

	if (get_env(env, "PWD"))
		return (1);
	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (0);
	if (add_env(env, "PWD", pwd, EXPORTED) == ENV_ERROR)
	{
		free(pwd);
		return (0);
	}
	free(pwd);
	return (1);
}

/**
 * @brief Initializes environment hash table from envp.
 *
 * @note Ensures mandatory variables (_ , SHLVL, PWD) exist.
 *
 * @param mms Mini-shell context structure
 * @param envp Null-terminated environment array
 *
*/

int	set_exported_env_ht(t_mms *mms, char **envp)
{
	char	*cwd;
	char	*path;
	int		status;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (0);
	path = ft_strjoin(cwd, "/minishell");
	free(cwd);
	if (!path)
		return (0);
	status = env_arr_to_ht(envp, mms->env);
	if (status != 0)
		status = init_underscore(mms->env, path);
	free(path);
	if (!status || !init_shlvl(mms->env))
		return (0);
	return (init_pwd(mms->env));
}

/**
 * @brief Add intern shell variable in environment hash table.
 *
 * @note Ensures PATH exist.
 *
 * @param mms Mini-shell context structure
 *
*/
int	set_var_env_ht(t_mms *mms)
{
	if (!get_env(mms->env, "PATH"))
		add_env(mms->env, "PATH", "/usr/local/bin:/usr/bin", 0);
	return (1);
}
