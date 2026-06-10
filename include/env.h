/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:25:31 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 17:08:22 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

# define DELETED ((void *)-1)

typedef enum e_flags
{
    EXPORTED,
    READONLY,
}   t_flags;

typedef enum e_val
{
       ERROR,
       SUCCESS,
       EMPTY,
       FULL,
} t_val;

typedef struct s_entry
{
    char    *key;
    char    *value;
    int     flags;
}   t_entry;

typedef struct s_env
{
    t_entry     *indexes;
    size_t      capacity;
    size_t      entries;
    
}   t_env;

t_val   add_env(t_env *env, char *key, char *str, int flags);
t_env    *init_env(size_t n);
t_val   del_env(t_env *env, char *key);
t_entry *get_env(t_env *env, char *key);
void    free_env(t_env *env);

#endif