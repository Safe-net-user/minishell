/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:30:51 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/10 19:43:52 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*#include <readline/readline.h>
#include <readline/history.h>*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "minishell.h"
#include "ft_memory.h"
#include "ft_string_builder.h"
#include "ft_strings.h"
#include "ft_stdlib.h"

/**
 * A noter que si HOME est change, alors la ht des commandes doit etre videe.
 * Penser a revenir sur cette taille car on risque largement de cache miss. (n x sizeof(type))
 * Penser aux conditions si une allocation rate.
 */
static	int	init_og_struct(t_mms *mms)
{
	mms = malloc(sizeof(t_mms));
	if (!mms)
		return (0);
	mms->env = init_env(INIT_SIZE_HT);
	mms->alias = init_hash_table(INIT_SIZE_HT);
	mms->cmd_path = init_hash_table(INIT_SIZE_HT);
	mms->sa = init_stack_allocator(INIT_SIZE_SA);
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
static int	set_exported_env_ht(t_mms *mms, char **envp)
{
	t_env_entry	*p;
	int			shell_lvl;
	t_env_flags	flags;
	
	flags = 0;
	if (arr_to_ht(envp, mms->env) == 0)
		return (0);
	if (!get_env(mms->env, "_"))
		add_env(mms->env, "_", "/usr/bin/env", flags |= EXPORTED);
	p = get_env(mms->env, "SHLVL");
	if (!p)
		add_env(mms->env, "SHLVL", "1", flags |= EXPORTED);
	else
	{
		shell_lvl = ft_atoi(p->value) + 1;
		add_env(mms->env, "SHLVL", ft_itoa(shell_lvl), flags |= EXPORTED);
	}
	if (!get_env(mms->env, "PWD"))
		add_env(mms->env, "PWD", getcwd(NULL, 0), flags |= EXPORTED);
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
static int	set_env_ht(t_mms *mms)
{
	t_env_flags	flags;
	
	flags = 0;
	add_env(mms->env, "PS1", "'\\s-\\v\\$'", flags);
	add_env(mms->env, "PS2", "'> '", flags);
	add_env(mms->env, "PS4", "'+ '", flags);
	if (!get_env(mms->env, "PATH"))
		add_env(mms->env, "PATH", "/usr/local/bin:/usr/bin", flags);
	return (1);
}

static	int set_og_struct(t_mms *mms, char **envp)
{
	if (set_exported_env_ht(mms, envp) == 0)
		return (0);
	if (set_env_ht(mms) == 0)
		return (0);
	return (1);
}

/*
 *Si dans env il manque des variables comme PWD, OLDPWD, PATH etc il faudra aux moment cle les rajouter dans la ht var et non pas env
 
static	int	set_var_ht(t_mms *mms, char **envp)
{
	ft_memcpy(mms->var->entries, mms->env->entries, (INIT_SIZE_HT * sizeof(t_entry)));
	set_new_value(mms->var, "PS1", "'\\s-\\v\\$'");
	set_new_value(mms->var, "PS2", "'> '");
	set_new_value(mms->var, "PS4", "'+ '");
}*/

int main(UNUSED int ac, UNUSED char **av, char **envp)
{
	//char 	*result;
	t_mms	*mms;

	mms = NULL;
	if (init_og_struct(mms) == 0)
		return (1);
	else if (set_og_struct(mms, envp) == 0)
	{
		//free_og_struct(mms);
		return (1);
	}
	/*while (1) //de ici
	{
		result = readline("miniMishell$: ");
		if (!result)
			break;
		if (*result)
			add_history(result);
		//suite minishell
		free(result);
	} // a ici soit une fonction dite "REPL" soit on laisse dans le main*/
	return (0);
}

// compiler avec -lreadline%  