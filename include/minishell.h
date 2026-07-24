/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:49:14 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/24 11:36:22 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#define UNUSED __attribute__((unused))

# include <sys/types.h>
# include "ft_hashtable.h"
# include "ft_stack_alloc.h"
#include "type.h"
# include "env.h"
# include "parser.h"

# define INIT_SIZE_HT 1024
# define INIT_SIZE_SA 32768
# define DEFAULT_SB_SIZE 64

/**
 * @struct s_mms
 * @brief Global state of the minishell instance.
 *
 * Stores the shell execution environment and all runtime resources
 * required by the shell. This structure roughly follows the POSIX
 * Shell Execution Environment model while omitting unsupported
 * features such as shell functions, traps, job control and shell
 * options.
 *
 * Additional implementation-specific resources are also stored,
 * including command path hashtable and a stack allocator.
 */
struct s_mms
{
    t_env           *env;           /**< Environment variables */
    t_ht            *cmd_path;      /**< Command path */
    t_ht            *alias;         /**< Shell aliases*/
    t_stack_alloc   *sa;            /**< Stack allocator */
    char            *name;          /**< Shell executable name and value of special parameter $0 */
    char            *cwd;           /**< Current working directory */
    mode_t          umask;          /**< Process file creation mask */
    int             last_status;    /**< Value of the special parameter $? */
};

void    set_signaux_interactif(void); //signaux interactif


/* Free functions */
void    free_og_struct(t_mms *mms);
void	free_stack_allocator(t_stack_alloc *sa);
void	free_ast_values(t_ast *node);
void	stack_reset(t_stack_alloc *sa);

extern int g_signal; // la variable globale (signal)

#endif
