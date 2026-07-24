/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:07:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/13 12:32:30 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "env.h"
#include "ft_strings.h"
#include <unistd.h>
#include <stdlib.h>

static t_builts_val	update_pwd(t_env *env, char *old_pwd)
{
    t_env_entry	*entry;
    char		*new_pwd;

    new_pwd = getcwd(NULL, 0);
    if (!new_pwd)
        return (BUI_ERROR);
    entry = get_env(env, "OLDPWD");
    if (entry)
    {
        free(entry->value);
        entry->value = old_pwd;
    }
    else
    {
        add_env(env, "OLDPWD", old_pwd, EXPORTED);
        free(old_pwd);
    }
    entry = get_env(env, "PWD");
    if (entry)
    {
        free(entry->value);
        entry->value = new_pwd;
    }
    else
    {
        add_env(env, "PWD", new_pwd, EXPORTED);
        free(new_pwd);
    }
    return (BUI_SUCCESS);
}

t_builts_val	builtin_cd(t_env *env, char **argv)
{
    t_env_entry	*entry;
    char		*old_pwd;

    if (!argv)
        return (BUI_ERROR);
    if (argv[1] && argv[2])
        return (BUI_TOO_MANY_ARGS);
    old_pwd = getcwd(NULL, 0);
    if (!old_pwd)
        return (BUI_ERROR);
    if (!argv[1] || argv[1][0] == '\0')
    {
        entry = get_env(env, "HOME");
        if (!entry || !entry->value)
        {
            free(old_pwd);
            return (BUI_HOME_NOT_SET);
        }
        if (chdir(entry->value) != 0)
        {
            free(old_pwd);
            return (BUI_ERROR);
        }
    }
    else if (ft_strcmp(argv[1], "-") == 0)
    {
        entry = get_env(env, "OLDPWD");
        if (!entry || !entry->value)
        {
            free(old_pwd);
            return (BUI_OLDPWD_NOT_SET);
        }
        if (chdir(entry->value) != 0)
        {
            free(old_pwd);
            return (BUI_ERROR);
        }
    }
    else
    {
        if (chdir(argv[1]) != 0)
        {
            free(old_pwd);
            return (BUI_ERROR);
        }
    }
    return (update_pwd(env, old_pwd));
}