/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:42:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/13 16:26:10 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include <stdio.h>
#include "minishell.h"
#include "ft_strings.h"


static  t_builts_val print_exported_var(t_env *env)
{
    t_env_entry *arr;
    t_env_entry *curr_p;

    arr = malloc(sizeof(t_env_entry) * (env->entries + 1));
    if (!arr)
        return (BUI_ERROR);
    ht_to_arr(env, arr);
    if (!sort_arr(arr))
    {
        free(arr);
        return (BUI_ERROR);
    }
    curr_p = arr;
    while (curr_p->key)
    {
        if (curr_p->value)
            printf("export %s=\"%s\"\n", curr_p->key, curr_p->value);
        else
            printf("export %s\n", curr_p->key);
        curr_p++;
    }
    free(arr);
    return (BUI_SUCCESS);
}

static  t_builts_val alone_key_job(t_mms *mms, char *buffer_key) {
    t_env_entry *entry;

    entry = get_env(mms->env, buffer_key);
    if (!entry) {
        if (add_env(mms->env, buffer_key, NULL, EXPORTED))
            return (BUI_SUCCESS);
        return (BUI_ERROR);
    }
    entry->flags |= EXPORTED;
    return (BUI_SUCCESS);
}

static  t_builts_val assign(size_t i, size_t j, t_mms *mms, char *buffer_key, char *src)
{
    char            buffer_value[ENV_NAME_MAX];

    while (src[i] && src[i] != '=')
    {
        if (i >= ENV_KEY_MAX)
            return (BUI_INVALID_VAR_NAME_SIZE);
        buffer_key[i] = src[i];
        i++;
    }
    buffer_key[i] = '\0';
    if (!is_validname(buffer_key))
        return (BUI_INVALID_VAR_NAME);
    if (src[i] == '=')
        i++;
    else
        return (alone_key_job(mms, buffer_key));
    while (src[i])
    {
        if (j >= ENV_NAME_MAX)
            return (BUI_INVALID_VAR_NAME_SIZE);
        buffer_value[j++] = src[i++];
    }
    buffer_value[j] = '\0';
    if (!add_env(mms->env, buffer_key, buffer_value, EXPORTED))
        return (BUI_ERROR);
    return (BUI_SUCCESS);
}

static  t_builts_val parse_export_arg(t_mms *mms, char *src)
{
    t_builts_val    ret;
    char            buffer_key[ENV_KEY_MAX];
    size_t          i;
    size_t          j;

    i = 0;
    j = 0;
    ret = assign(i, j, mms, buffer_key, src);
    if (ret != BUI_SUCCESS)
        return (ret);
    if (ft_strcmp(buffer_key, "PATH") == 0) {
        free_hash_table(mms->cmd_path);
        mms->cmd_path = init_hash_table(INIT_SIZE_HT);
        if (!mms->cmd_path)
            return (BUI_ERROR);
    }
    return (BUI_SUCCESS);
}

t_builts_val     builtin_export(t_mms *mms, char **arg)
{
    int i;
    t_builts_val    ret;

    i = 1;
    if (!mms || !mms->env || !arg)
        return (BUI_ERROR);
    if (!arg[1])
        return (print_exported_var(mms->env));
    while (arg[i])
    {
        ret = parse_export_arg(mms, arg[i++]);
        if (ret != BUI_SUCCESS)
            return (ret);
    }
    return (BUI_SUCCESS);
}