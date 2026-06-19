/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:49:27 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/19 11:52:30 by gd-hallu         ###   ########.fr       */
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
	char		*path;
	int			shell_lvl;
	
	path = getcwd(NULL, 0);
	ft_strlcat(path, "minishell", 10);
	if (env_arr_to_ht(envp, mms->env) == 0)
		return (0);
	if (!get_env(mms->env, "_"))
		add_env(mms->env, "_", path, EXPORTED + READONLY);
	p = get_env(mms->env, "SHLVL");
	if (!p)
		add_env(mms->env, "SHLVL", "1", EXPORTED);
	else
	{
		shell_lvl = ft_atoi(p->value) + 1;
		add_env(mms->env, "SHLVL", ft_itoa(shell_lvl), EXPORTED);
	}
	if (!get_env(mms->env, "PWD"))
		add_env(mms->env, "PWD", getcwd(NULL, 0), EXPORTED);
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