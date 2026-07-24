/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ht_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:51:41 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 10:52:47 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include <stdlib.h>
#include "ft_strings.h"
#include "ft_crypto.h"
#include "ft_printf.h"
#include "ft_memory.h"
#include "ft_stdlib.h"
#include <stdio.h>

t_env    *init_env(size_t n)
{
    t_env    *env;

    if (n == 0 || n > SIZE_MAX / sizeof(t_env_entry))
        return (NULL);
    env = ft_calloc(1, sizeof(t_env_entry));
    if (!env)
        return (NULL);
    env->indexes = ft_calloc(n, sizeof(t_env_entry));
    if (!env->indexes)
    {
        free(env);
        return (NULL);
    }
    env->entries = 0;
    env->capacity = n;
    return (env);
}

t_env_val   add_env(t_env *env, char *key, char *str, t_env_flags flags)
{
    size_t      n;
    size_t      index;

    if (!env || !key)
        return (ENV_ERROR);
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

t_env_entry *get_env(t_env *env, char *key)
{
    size_t  n;
    size_t  index;
    size_t  tmp_index;

    if (!env || !key)
        return (NULL);
    n = ft_strlen(key);
    index = joaat_hash((unsigned char *)key, n) % env->capacity;
    tmp_index = (index - 1) % env->capacity;
    while (env->indexes[index].key != NULL)
    {
        if (env->indexes[index].key != DELETED)
        {
            if (ft_strncmp(env->indexes[index].key, key, n + 1) == 0)
                return (&env->indexes[index]);
        }
        if (__builtin_expect(tmp_index == index, 0))
            return (NULL);
        index = (index + 1) % env->capacity;
    }
    return (NULL);
}

void    free_env(t_env *env)
{
    size_t  i;

    if (!env)
        return ;
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
}

t_env_val   del_env(t_env *env, char *key)
{
    size_t      n;
    size_t      index;

    if (!env || !key)
        return (ENV_ERROR);
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
            free(env->indexes[index].key);
            env->indexes[index].key = DELETED;
            return (ENV_SUCCESS);
        }
        index = (index + 1) % env->capacity;
    }
    return (ENV_ERROR);
}