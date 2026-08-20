/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 23:38:26 by miouali           #+#    #+#             */
/*   Updated: 2026/08/20 23:42:34 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include <unistd.h>
#include <stdlib.h>
#include "ft_io.h"

int	export_internal_error(t_env_entry *arr)
{
	if (arr)
		free(arr);
	ft_putstr_fd("miniMishell: export: internal error\n",
		STDERR_FILENO);
	return (1);
}

t_env_entry	*build_sorted_arr(t_env *env)
{
	t_env_entry	*arr;

	arr = malloc(sizeof(*arr) * (env->entries + 1));
	if (!arr)
		return (NULL);
	ht_to_arr(env, arr);
	if (!sort_arr(arr))
	{
		free(arr);
		return (NULL);
	}
	return (arr);
}
