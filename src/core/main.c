/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:30:51 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/09 10:34:54 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

int main(int ac, char **av, char **envp)
{
	char *result;

    //set_variables, hash_table, env etc
    
	while (1) //de ici
	{
		result = readline("minishell$: ");
		if (!result)
			break;
		if (*result)
			add_history(result);
		//suite minishell
		free(result);
	} // a ici soit une fonction dite "REPL" soit on laisse dans le main
	return (0);
}

// compiler avec -lreadline%  