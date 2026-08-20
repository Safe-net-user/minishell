/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 13:06:37 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:04:39 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file echo.c
 * @brief `echo` builtin implementation.
 *
 * `builtin_echo()` prints its arguments separated by single spaces.
 * Leading arguments matching `-n`/`-nn`/`-nnn`/... (checked by
 * `n_option_isvalid()`) suppress the trailing newline, matching
 * bash's non-POSIX `-n` extension; option parsing stops at the
 * first argument that isn't a valid `-n` flag, so anything after
 * that is treated as plain output.
 */

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
