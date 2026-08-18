/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_errors.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 17:01:15 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/18 18:59:07 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"

void	print_exec_error(char *path, char *msg)
{
	write(2, "miniMishell: ", 13);
	if (path)
		write(2, path, ft_strlen(path));
	write(2, ": ", 2);
	if (msg)
		write(2, msg, ft_strlen(msg));
	write(2, "\n", 1);
}

void	print_error(char *msg)
{
	write(2, "miniMishell: ", 13);
	if (msg)
		write(2, msg, ft_strlen(msg));
	write(2, "\n", 1);
}

void	print_cmd_not_found(char *cmd)
{
	write(2, "miniMishell: ", 13);
	if (cmd)
		write(2, cmd, ft_strlen(cmd));
	write(2, ": command not found\n", 20);
}
