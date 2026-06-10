/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 17:05:34 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 19:47:42 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "env.h"

void    free_env(t_env *env)
{
    size_t  i;

    i = 0;
    while (i < env->capacity)
    {
        if (env->indexes[i].key != NULL  && env->indexes[i].key != DELETED)
        {
            free(env->indexes[i].value);
            free(env->indexes[i].key);
        }
        i++;
    }
    free(env->indexes);
    free(env);
    env = NULL;
}