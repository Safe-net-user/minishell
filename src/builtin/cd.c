/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:07:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/11 11:22:06 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "env.h"

/*en gros ici dans cd on a plusieurs cas d'office :
- le cas cd
- le cas ou ca commence par ~
- le cas cd -
- le reste est gerer avec chdir()*/

int builtins_cd(t_env *env, char *path)
{
    char    *actual_pwd;
    t_env_entry *entry;
    
    actual_pwd = getcwd(actual_pwd, 1024);
    if (!path || strncmp(path, "~") == 0) //cd tout seul || cd ~
    {
        if (chdir(get_env(env, "HOME") != 0)) //chercher d'ans l'env le HOME
            return (-1);
    }
    else
    {
        if (chdir(path) != 0)
            return (-1);
    }
    entry = get_env(env, "PWD"); //upodate pwd
    if (entry)
        entry->value = actual_pwd;
    actual_pwd = getcwd(actual_pwd, 1024);
    entry = get_env(env, "OLD_PWD"); //update old_pwd
    if (entry)
        entry->value = actual_pwd;
    return (0);
}
