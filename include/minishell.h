/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:49:14 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/20 16:37:47 by miouali          ###   ########.fr       */
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
	t_ast			*current_ast;
	char			*name;
	char			*cwd;
	mode_t			umask;
	int				tty_fd;
	int				should_exit;
	int				exit_status;
	int				last_status;
};

/* Signals */

void	set_signaux_interactif(void);
void	set_signaux_heredoc(void);

/* Free functions */

void	free_og_struct(t_mms *mms);
void	free_stack_allocator(t_stack_alloc *sa);
void	free_ast_values(t_ast *node);
int		cleanup_and_exit(t_mms *mms);
void	stack_reset(t_stack_alloc *sa);

/* Input */

bool	is_blank(const char *s);
char	*read_line(void);
int		process_input(t_mms *mms);

/* Parser and executor */

void	run_parser_and_exec(t_mms *mms);
int		get_exit_status(t_mms *mms);

extern int	g_signal;

#endif
