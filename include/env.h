/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:25:31 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 19:44:42 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

#include <stddef.h>

# define DELETED ((void *)-1)

typedef enum e_env_flags
{
    EXPORTED = 1 << 0,
    READONLY = 1 << 1,
}   t_env_flags;

typedef enum e_env_val
{
    ENV_ERROR,
    ENV_SUCCESS,   
    ENV_EMPTY,
    ENV_FULL,
} t_env_val;

typedef struct s_env_entry
{
    char    *key;
    char    *value;
    int     flags;
}   t_env_entry;

typedef struct s_env
{
    t_env_entry     *indexes;
    size_t          capacity;
    size_t          entries;
    
}   t_env;

t_env_val   add_env(t_env *env, char *key, char *str, t_env_flags flags);
t_env       *init_env(size_t n);
t_env_val   del_env(t_env *env, char *key);
t_env_entry *get_env(t_env *env, char *key);
void        free_env(t_env *env);

#endif