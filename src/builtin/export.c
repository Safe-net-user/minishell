/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 11:42:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/12 13:39:14 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include"env.h"

//important, chaque nouvelle exportee vient au dessus de la liste et non en dessous.
// export seul affiche toutes les variables exportees
// export VAR=valeur → crée/modifie + flag EXPORTED
// export VAR → flag EXPORTED sur variable existante
// dans env flags on a un EXPORT 1 ou 0
int     export(t_env *env, char *arg)
{
    if (!arg)
    {
        print_env(env);
        return (0);
    }
    if ()
}