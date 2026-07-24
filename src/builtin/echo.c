/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 13:06:37 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 23:35:07 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ft_io.h"
#include "ft_strings.h"
#include "builtin.h"

int	n_option_isvalid(char *str)
{
	size_t	i;

	i = 0;
	if (!str || str[i] != '-')
		return (0);
	i++;
	if (str[i] != 'n')
		return (0);
	while (str[i] == 'n')
		i++;
	if (str[i] == '\0')
		return (1);
	return (0);
}

t_builts_val	builtin_echo(char **tab)
{
	bool	new_line_active;
	int		i;

	if (!tab)
	{
		ft_putstr_fd("miniMishell: echo: internal error\n", STDERR_FILENO);
		return (1);
	}
	new_line_active = 1;
	i = 1;
	while (tab[i] && n_option_isvalid(tab[i]))
	{
		new_line_active = 0;
		i++;
	}
	while (tab[i])
	{
		write(STDOUT_FILENO, tab[i], ft_strlen(tab[i]));
		if (tab[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (new_line_active)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}
