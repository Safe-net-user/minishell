/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils_1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 18:00:45 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/24 18:02:35 by gd-hallu         ###   ########.fr       */
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

char	*get_old_pwd(t_mms *mms)
{
	char	*old_pwd;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		old_pwd = ft_strdup(mms->cwd);
	return (old_pwd);
}

t_builts_val	update_pwd(t_mms *mms, char *old_pwd)
{
	char	*str;

	str = getcwd(NULL, 0);
	if (!str)
	{
		free(old_pwd);
		return (internal_error());
	}
	add_env(mms->env, "OLDPWD", old_pwd, EXPORTED);
	add_env(mms->env, "PWD", str, EXPORTED);
	free(mms->cwd);
	mms->cwd = str;
	free(old_pwd);
	return (BUI_SUCCESS);
}
