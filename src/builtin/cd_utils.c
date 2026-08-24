/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 12:07:56 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 02:06:03 by gaspard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "minishell.h"
#include "env.h"
#include "ft_strings.h"
#include "ft_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

t_builts_val	internal_error(void)
{
	ft_putstr_fd("miniMishell: cd: internal error\n", STDERR_FILENO);
	return (BUI_ERROR);
}

t_builts_val	variable_not_set_error(void)
{
	ft_putstr_fd("miniMishell: cd: variable not set\n", STDERR_FILENO);
	return (BUI_ERROR);
}

t_builts_val	too_many_args_error(void)
{
	ft_putstr_fd("miniMishell: cd: too many arguments", STDERR_FILENO);
	return (BUI_TOO_MANY_ARGS);
}

char	*get_new_path(t_mms *mms, char *old_pwd, char *path)
{
	char	*tmp;
	char	*final_path;

	tmp = NULL;
	final_path = NULL;
	if (path && path[0] == '/')
		return (path);
	if (!old_pwd)
	{
		if (mms->cwd && mms->cwd[ft_strlen(mms->cwd) - 1] != '/')
			tmp = ft_strjoin(mms->cwd, "/");
		else
			tmp = ft_strdup(mms->cwd);
		final_path = ft_strjoin(tmp, path);
		free(tmp);
		free(mms->cwd);
		mms->cwd = ft_strdup(final_path);
		free(final_path);
		return (path);
	}
	tmp = ft_strjoin(old_pwd, "/");
	final_path = ft_strjoin(tmp, path);
	free(tmp);
	return (final_path);
}
