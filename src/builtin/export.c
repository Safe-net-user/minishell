/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:42:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/11 19:02:30 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include"env.h"

//important, chaque nouvelle exportee vient au dessus de la liste et non en dessous.

int     export(t_env *env, char *arg)
{
    if (!arg)
    {
        print_env(env);
        return (0);
    }
    if ()
}