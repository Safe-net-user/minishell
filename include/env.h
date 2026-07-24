/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 16:25:31 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 14:59:08 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

#include <stddef.h>
#include "type.h"

# define DELETED ((void *)-1)

# define ENV_KEY_MAX 256
# define ENV_NAME_MAX 8192

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
    int     place;
}   t_env_entry;

struct s_env
{
    t_env_entry *indexes;
    size_t      capacity;
    size_t      entries;
};

/* Hashtable functions */
t_env_val   add_env(t_env *env, char *key, char *str, t_env_flags flags);
t_env       *init_env(size_t n);
t_env_val   del_env(t_env *env, char *key);
t_env_entry *get_env(t_env *env, char *key);
void        free_env(t_env *env);

/* Converter functions */
int         env_arr_to_ht(char **envp, t_env *ht);
char	**env_to_envp(t_env *env);

/* Setter functions */
int	set_exported_env_ht(t_mms *mms, char **envp);
int	set_var_env_ht(t_mms *mms);

#endif