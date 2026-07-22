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

#include <unistd.h>
#include "env.h"

/*en gros ici dans cd on a plusieurs cas d'office :
- le cas cd
- le cas ou ca commence par ~
- le cas cd -
- le reste est gerer avec chdir()*/

//return 2 si trop arg
// return 1 si pas trouve
// sinon return 0

int builtins_cd(t_env *env, char **arg)
{
    char    *actual_pwd;
    t_env_entry *entry;
    int i;

    while (arg[i])
        i++;
    if (i > 2)
        return (2); //valeur de retour pour g_signal 
    actual_pwd = getcwd(actual_pwd, 1024);
    if (!arg || strncmp(arg[1], "~") == 0) //cd tout seul || cd ~
    {
        if (chdir(get_env(env, "HOME") != 0)) //chercher d'ans l'env le HOME
            return (-1);
    }
    else
    {
        if (chdir(arg[1]) != 0)
            return (1);
    }
    entry = get_env(env, "OLD_PWD"); //update old_pwd
    if (entry)
        entry->value = actual_pwd;
    actual_pwd = getcwd(actual_pwd, 1024);
    entry = get_env(env, "PWD"); //update pwd
    if (entry)
        entry->value = actual_pwd;
    return (0);
}