/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:57:28 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 19:48:03 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include <stdlib.h>
#include "ft_crypto.h"
#include "ft_strings.h"

t_env_entry *get_env(t_env *env, char *key)
{
    size_t  n;
    size_t  index;
    size_t  tmp_index;

    n = ft_strlen(key);
    index = joaat_hash((unsigned char *)key, n) % env->capacity;
    tmp_index = index - 1;
    while (env->indexes[index].key != NULL)
    {
        if (env->indexes[index].key != DELETED)
            if (ft_strncmp(env->indexes[index].key, key, n + 1) == 0)
                return (&env->indexes[index]);
        if (__builtin_expect(tmp_index == index, 0))
            return (NULL);
        index = (index + 1) % env->capacity;
    }
    return (NULL);
}