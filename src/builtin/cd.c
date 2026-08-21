/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:07:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:04:22 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "env.h"
#include "ft_strings.h"
#include "ft_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

t_builts_val    internal_error(void)
{
    ft_putstr_fd("miniMishell: cd: internal error\n", STDERR_FILENO);
    return (BUI_ERROR);
}

t_builts_val    variable_not_set_error(void)
{
    ft_putstr_fd("miniMishell: cd: variable not set\n", STDERR_FILENO);
    return (BUI_ERROR);
}

t_builts_val    too_many_args_error(void)
{
    ft_putstr_fd("miniMishell: cd: too many arguments", STDERR_FILENO);
    return (BUI_TOO_MANY_ARGS);
}
t_builts_val    cd_home(t_mms *mms)
{
    t_en_entry  *entry;

    entry = get_env(mms->env, "HOME");
    if (!entry || !entry->str)
        return (variable_not_set_error());
    if (chdir(entry->str) < 0)
    {
        perror("miniMishell: cd");
        return (BUI_ERROR);
    }
    mms->cwd = entry->str;
    return (BUI_SUCCESS);
}

t_builts_val    cd(t_mms *mms, char *path)
{
    t_env_entry *entry;
    char        *old_pwd;

    old_pwd = getcwd(NULL, 0);
    if (!old_pwd)
    {
        ft_strjoin(mms->cwd, path);
        if (chdir(mms->cwd) < 0)
        {
            perror("miniMishell: cd");
            add_env(mms->env, OLDPWD, mms->cwd);
            return (BUI_ERROR);
        }
        return (BUI_SUCCESS);
    }
    add_env(mms->env, OLD_PWD, old_pwd);
}

t_builts_val    builtin_cd(t_mms *mms, char **av)
{
    if (!mms || !mms->env || !av)
        return (internal_error());
    if (av[2])
        return (too_many_args_error());
    if (!av[1])
        return (cd_home(mms));
    return (cd(mms, av[1]));

}