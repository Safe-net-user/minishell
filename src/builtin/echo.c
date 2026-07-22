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

static int check_flag(char **str)
{
    int i;
    int j;
    int count_flag = 0;
    
    i = 2;
    j = 0;
    while (str[i])
    {
        while (str[i][j])
        {
            if (str[i][j] == '-')
            {
                j++;
                if (str[i][j] == 'n')
                {    
                    while (str[i][j])
                    {
                        if (str[i][j] != 'n')
                            return (count_flag);
                        j++;
                    }
                    count_flag++;
                }
                else
                    return (count_flag);
            }
            else
                return (count_flag);
        }
        i++;
    }
    return (count_flag);
}

int     builtin_echo(char **tab)
{
    int i;
    int flags;

    flags = check_flag(tab);
    i = 2 + flags;
    while (tab[i])
    {
        printf("%s", tab[i]);
        i++;
    }
    if (flags == 0)
        printf ("\n");
    return (0);
}

int main (int ac, char **av)
{
    if (ac < 2)
    {
        return (1);
    }
    if (strncmp(av[1], "echo", 5) == 0)
    {
        builtin_echo(av);
    }
    else
        printf ("caca");
}