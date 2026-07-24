/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   converter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 14:01:35 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 10:51:31 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "ft_strings.h"
#include <stdio.h>
#include <stdlib.h>

int	env_arr_to_ht(char **envp, t_env *ht)
{
	size_t	i;
	size_t	j;
	char	*start;

	i = 0;
	while (envp[i])
	{
		j = 0;
		start = ft_strdup(envp[i]);
		if (!start)
			return (0);
		while (start[j] != '=')
			j++;
		start[j] = '\0';
		j++;
		add_env(ht, start, &start[j], EXPORTED);
		free(start); // CHANGE: add_env copie key/value en interne (ft_strdup), donc start doit être libéré ici
		i++;
	}
	return (1);
}