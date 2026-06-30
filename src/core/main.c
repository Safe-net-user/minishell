/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:30:51 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/11 21:14:00 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include "minishell.h"
#include "ft_memory.h"
#include "ft_string_builder.h"
#include "ft_strings.h"
#include "env.h"
#include <signal.h>

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
	char 	*result;
	t_mms	*mms;
	
	mms = init_og_struct();
	if (!mms || set_og_struct(mms, envp) == 0)
	{
		free_og_struct(mms);
		return (EXIT_FAILURE);
	}
	set_signaux_interactif();
	while (1) //de ici
	{
		result = readline("miniMishell$: ");
		printf ("arg : %s\n", result);
		if (!result)
		{
    		if (g_signal == SIGINT)  // Ctrl+C
    		{
        		g_signal = 0;
        		continue;
    		}
    		break;  // vrai Ctrl+D
		}
		if (*result)
			add_history(result);
		//suite minishell
		free(result);
	} // a ici soit une fonction dite "REPL" soit on laisse dans le main
	return (0);
}

// compiler avec -lreadline  