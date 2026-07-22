/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 13:06:37 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/05/28 11:50:15 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <curses.h>
#include <stdio.h>
#include <string.h>

t_builts_val builtin_echo(char **tab)
{
    bool new_line_active;
    int i;

    if (!tab)
        return (ERROR);
    new_line_active = 1;
    i = 1;
    if (!tab[1])
        write(1, "\n", 1);
    if (ft_strcmp(tab[1], "-n") == 0)
        new_line_active = 0; i++;
    while (tab[i])
        write(1, tab[i], ft_strlen(tab[i++]));
    if (new_line_active)
        write(1, "\n", 1);
    return (SUCCESS);
}