/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:30:51 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/11 15:45:11 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*#include <readline/readline.h>
#include <readline/history.h>*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "minishell.h"
#include "ft_memory.h"
#include "ft_string_builder.h"
#include "ft_strings.h"
#include "env.h"

/**
 * A noter que si HOME est change, alors la ht des commandes doit etre videe.
 * Penser a revenir sur cette taille car on risque largement de cache miss. (n x sizeof(type))
 * Penser aux conditions si une allocation rate.
 */
static	t_mms	*init_og_struct(void)
{
	t_mms	*mms;
	
	mms = malloc(sizeof(t_mms));
	if (!mms)
		return (0);
	mms->env = init_env(INIT_SIZE_HT);
	mms->alias = init_hash_table(INIT_SIZE_HT);
	mms->cmd_path = init_hash_table(INIT_SIZE_HT);
	mms->sa = init_stack_allocator(INIT_SIZE_SA);
	mms->cwd = getcwd(NULL, 0);
	mms->name = ft_strdup("miniMishell");
	mms->last_status = 0;
	mms->umask = 0022;
	if (!mms->env || !mms->alias || !mms->cmd_path || !mms->sa || !mms->name)
	{
		free_og_struct(mms);
		return (NULL);
	}
	return (mms);
}

static	int set_og_struct(t_mms *mms, char **envp)
{
	if (!mms || !envp)
		return (0);
	if (set_exported_env_ht(mms, envp) == 0)
		return (0);
	if (set_var_env_ht(mms) == 0)
		return (0);
	return (1);
}

int main(UNUSED int ac, UNUSED char **av, char **envp)
{
	//char 	*result;
	t_mms	*mms;
	
	mms = init_og_struct();
	if (!mms || set_og_struct(mms, envp) == 0)
	{
		free_og_struct(mms);
		return (EXIT_FAILURE);
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