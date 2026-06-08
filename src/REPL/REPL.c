/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   REPL.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:41:32 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/05 16:25:45 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>

char **split_env(char *str, char first_sep) //split dedie juste pour l'env a voir si il faut forcemetn malloc les char **
{
	char	**env;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (str[i] && str[i] != first_sep)
		i++;
	if (str[i] == first_sep && str[i + 1])
	{
		while (j < i)
		{
			env[0][j] == str[j];
			j++;
		}
		i++;
		j = 0;
		while (i < strlen(str))
		{
			env[1][j] = str[i];
			j++;
			i++;
		}
		return (env);
	}
	else
		return (NULL);
}

//envoyer la hash table ou la creer ici

void	add_env(char **envp)
{
	int i;
	char	**split;

	i = 0;
	while (envp[i])
	{
		split = split_env(envp[i], '=');
		set_new_value(ht, split[0], split[1]); //possibilte de FULL ici ou de ERROR,a voir comment gerer.
		i++;
	}
	return;
}
