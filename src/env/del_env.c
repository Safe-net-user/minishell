/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   del_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 17:03:02 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 19:47:25 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include <stdlib.h>
#include "ft_crypto.h"
#include "ft_strings.h"
#include "ft_memory.h"

t_env_val   del_env(t_env *env, char *key)
{
    size_t      n;
    size_t      index;

    if (env->entries == 0)
        return (ENV_EMPTY);
    n = ft_strlen(key);
    index = joaat_hash((unsigned char *)key, n) % env->capacity;
    while (env->indexes[index].key != NULL)
    {
        if (ft_memcmp(env->indexes[index].key, key, n + 1) == 0)
        {
            env->entries--;
            free(env->indexes[index].value);
            env->indexes[index].key = DELETED;
            return (ENV_SUCCESS);
        }
        index = (index + 1) % env->capacity;
    }
    return (ENV_ERROR);
}