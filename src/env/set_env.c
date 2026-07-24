/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:49:27 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 10:43:48 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include "ft_strings.h"
#include "ft_stdlib.h"
#include "env.h"
#include <unistd.h>


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
	t_env_entry	*p;
	char		*cwd;
	char		*path;
	char		*lvl_str;
	char		*pwd;
	int			shell_lvl;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (0);
	path = ft_strjoin(cwd, "/minishell");
	free(cwd);
	if (!path)
		return (0);
	if (env_arr_to_ht(envp, mms->env) == 0)
	{
		free(path);
		return (0);
	}
	if (!get_env(mms->env, "_"))
		add_env(mms->env, "_", path, EXPORTED + READONLY);
	free(path);
	p = get_env(mms->env, "SHLVL");
	if (!p)
		add_env(mms->env, "SHLVL", "1", EXPORTED);
	else
	{
		shell_lvl = ft_atoi(p->value) + 1;
		lvl_str = ft_itoa(shell_lvl);
		add_env(mms->env, "SHLVL", lvl_str, EXPORTED);
		free(lvl_str);
	}
	if (!get_env(mms->env, "PWD"))
	{
		pwd = getcwd(NULL, 0);
		add_env(mms->env, "PWD", pwd, EXPORTED);
		free(pwd);
	}
	return (1);
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
	add_env(mms->env, "PS1", "miniMishell$ ", 0);
	add_env(mms->env, "PS2", "'> '", 0);
	if (!get_env(mms->env, "PATH"))
		add_env(mms->env, "PATH", "/usr/local/bin:/usr/bin", 0);
	return (1);
}