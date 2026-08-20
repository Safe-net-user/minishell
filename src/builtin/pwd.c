/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:24:11 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:05:35 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file pwd.c
 * @brief `pwd` builtin implementation.
 *
 * `builtin_pwd()` prints the current working directory via a fresh
 * `getcwd()` call rather than reading `$PWD` from the environment —
 * this guarantees the printed path reflects reality even if `$PWD`
 * is stale or was tampered with by the user.
 */

#include "builtin.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ft_io.h"

t_builts_val	builtin_pwd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		perror("miniMishell: pwd: ");
		return (1);
	}
	ft_putstr_fd(pwd, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	free(pwd);
	return (0);
}
