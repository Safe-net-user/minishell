/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:43:27 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 19:46:49 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include <stdlib.h>
#include "ft_strings.h"
#include "ft_crypto.h"

t_env_val   add_env(t_env *env, char *key, char *str, t_env_flags flags)
{
    size_t      n;
    size_t      index;

    if (env->entries == env->capacity)
        return (ENV_FULL);
    n = ft_strlen(key);
    index = joaat_hash((unsigned char *)key, n) % env->capacity;
    while (env->indexes[index].key != NULL)
    {
        if (ft_strncmp(env->indexes[index].key, key, n) == 0)
        {
            free(env->indexes[index].value);
            env->indexes[index].value = ft_strdup(str);
            env->indexes[index].flags = flags;
            return (ENV_SUCCESS);
        }
        index = (index + 1) % env->capacity;
    }
    env->indexes[index].value = ft_strdup(str);
    env->indexes[index].key = ft_strdup(key);
    env->indexes[index].flags = flags;
    env->entries++;
    return (ENV_SUCCESS);
}