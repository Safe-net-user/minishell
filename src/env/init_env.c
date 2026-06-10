/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_ht.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:37:55 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 17:06:44 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_stdlib.h"
#include "env.h"
#include <stdint.h>

t_env    *init_env(size_t n)
{
    t_env    *env;

    if (n == 0 || n > SIZE_MAX / sizeof(t_entry))
        return (NULL);
    env = ft_calloc(1, sizeof(t_entry));
    if (!env)
        return (NULL);
    env->indexes = ft_calloc(n, sizeof(t_entry));
    if (!env->indexes)
    {
        free(env);
        return (NULL);
    }
    env->entries = 0;
    env->capacity = n;
    return (env);
}