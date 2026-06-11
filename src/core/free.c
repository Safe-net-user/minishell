/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 14:43:40 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/11 15:46:40 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    free_og_struct(t_mms *mms)
{
    if (!mms)
        return ;
    if (mms->alias != NULL)
        free_hash_table(mms->alias);
    if (mms->cmd_path != NULL)
        free_hash_table(mms->cmd_path);
    if (mms->env != NULL)
        free_env(mms->env);
    if (mms->name != NULL)
        free(mms->name);
    free(mms);
}