/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:49:14 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/25 03:36:08 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <sys/types.h>
# include "ft_hashtable.h"
# include "ft_stack_alloc.h"
# include "type.h"
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
	t_env			*env;
	t_ht			*cmd_path;
	t_ht			*alias;
	t_stack_alloc	*sa;
	char			*name;
	char			*cwd;
	mode_t			umask;
	int				tty_fd;
	t_ast			*current_ast;
	int				last_status;
};

void	set_signaux_interactif(void);

/* Free functions */
void	free_og_struct(t_mms *mms);
void	free_stack_allocator(t_stack_alloc *sa);
void	free_ast_values(t_ast *node);
void	stack_reset(t_stack_alloc *sa);
bool	is_blank(const char *s);
char	*read_line(void);

extern int	g_signal;

#endif
