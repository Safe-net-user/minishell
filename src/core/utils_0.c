/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_0.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 17:19:57 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/18 14:11:48 by miouali          ###   ########.fr       */
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

	//if (isatty(STDIN_FILENO))
		line = readline("miniMishell$: ");
	//else
		//line = readline("");
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

void	run_loop(t_mms *mms)
{
	while (1)
	{
		stack_reset(mms->sa);
		g_signal = 0;
		if (!process_input(mms))
			break ;
	}
}

