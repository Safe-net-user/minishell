/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:24:11 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/11 11:38:51 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

t_builts_val builtin_pwd(void)
{
	char *pwd;

    pwd = getcwd(NULL, 0);
    if (!pwd)
    {
        perror("miniMishell: pwd: ");
        return (1);
    }
    printf("%s\n", pwd);
    free(pwd);
    return (0);
}
