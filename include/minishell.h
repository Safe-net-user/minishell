/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:49:14 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/11 15:04:23 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#define UNUSED __attribute__((unused))

# include <sys/types.h>
# include "ft_hashtable.h"
# include "ft_stack_alloc.h"
# include "env.h"

/*
 * Macro ARG_MAX designe ici le nombres de bytes max autorises dans les variables d'environement
 * On retrouve cette limite dans <sys/limits.h>, L'erreur retourne est E2BIG (<sys/errno.h>)
 * #ifndef ARG_MAX
 *  # define ARG_MAX 2097152
 * #endif
*/
# define INIT_SIZE_HT 1024
# define INIT_SIZE_SA 32768

typedef struct s_env t_env;

typedef struct s_mms
{
    t_env           *env;       // variable VAR (env aussi dedans mais portant un flag se quim permet de les skips)
    t_ht            *cmd_path;  // la par contre c pas posix norme car il s agit du path des cmd deja trouve si path non modifie
    t_ht            *alias;     // ht pour les alias ca c posix norme
    t_stack_alloc   *sa;        // stacka lloc ca c pas posix norme
    char            *name;      // name pour ?0
    char            *cwd;
    mode_t          umask;      // umask posix norme
    int           last_status;// pour $?
}   t_mms;

/* Free functions */
void    free_og_struct(t_mms *mms);

#endif
