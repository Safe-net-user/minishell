/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 23:36:26 by gaspard           #+#    #+#             */
/*   Updated: 2026/08/14 21:42:10 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "ft_strings.h"
#include "minishell.h"
#include "ft_stdlib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "ft_io.h"
#include <unistd.h>

static int	is_valid_arg(char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	if ((str[0] == '+' || str[0] == '-')
		&& ft_isdigit(str[1]))
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	request_exit(t_mms *mms, int status)
{
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	mms->should_exit = 1;
	mms->exit_status = status;
}

t_builts_val	builtin_exit(t_mms *mms, char **args)
{
	if (!mms || !args)
	{
		ft_putstr_fd("minishell: exit: internal error\n", STDERR_FILENO);
		return (BUI_ERROR);
	}
	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n",
			STDERR_FILENO);
		return (BUI_TOO_MANY_ARGS);
	}
	if (!args[1])
		request_exit(mms, mms->last_status);
	else if (!is_valid_arg(args[1]))
		request_exit(mms, 255); //2 pas 255 enfin je crois
	else
		request_exit(mms, ft_atoi(args[1]) % 256);
	return (BUI_SUCCESS);
}
