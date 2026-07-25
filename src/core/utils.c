/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 17:19:57 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/25 02:13:26 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_strings.h"
#include <unistd.h>
#include <readline/readline.h>
#include <signal.h>

char	*read_line(void)
{
	char	*line;

	if (isatty(STDIN_FILENO))
		line = readline("miniMishell$: ");
	else
		line = readline("");
	if (g_signal == SIGINT)
	{
		free(line);
		return (NULL);
	}
	return (line);
}

int	ft_isspace(int c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}

bool	is_blank(const char *s)
{
	while (*s)
	{
		if (!ft_isspace((unsigned char)*s))
			return (false);
		s++;
	}
	return (true);
}
