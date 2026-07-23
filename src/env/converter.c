/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   converter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 14:01:35 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/12 11:02:24 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "ft_strings.h"
#include <stdio.h>
#include <stdlib.h>

int env_arr_to_ht(char **envp, t_env *ht)
{
	size_t		i;
	size_t		j;
	char		*start;
	
	i = 0;
	while(envp[i])
	{
        j = 0;
		start = ft_strdup(envp[i]);
		while (start[j] != '=')
			j++;
		start[j] = '\0';
		j++;
		add_env(ht, start, &start[j], EXPORTED);
		i++;
	}
	return (1);
}