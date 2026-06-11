/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:07:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/10 11:53:49 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

//ici peut etre on va recuperer depuis la structure pwd donc pas besoin de getcwd


/*en gros ici dans cd on a plusieurs cas d'office :
- le cas cd
- le cas ou ca commence par ~
- le cas cd -
- le reste est gerer avec chdir()*/

int builtins_cd(char *path)
{
    char    *actual_pwd;
    
    
    
    
    
    actual_pwd = (actual_pwd, 1024);
    ft_strjoin(actual_pwd, '/');
    ft_strjoin(actual_pwd, path);
    if (chdir(actual_pwd) != 0)
        return (-1);
    //update old_pwd & pwd;
    return (0);
}