/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:24:11 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/23 17:25:40 by gd-hallu         ###   ########.fr       */
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
#include "minishell.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ft_io.h"

t_builts_val	builtin_pwd(t_mms *mms)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		if (mms->cwd)
		{
			ft_putstr_fd(mms->cwd, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
			return (BUI_SUCCESS);
		}
		perror("miniMishell: pwd: ");
		return (BUI_ERROR);
	}
	ft_putstr_fd(pwd, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	free(pwd);
	return (BUI_SUCCESS);
}
