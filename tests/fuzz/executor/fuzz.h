/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 13:56:26 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 13:56:27 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FUZZ_H
# define FUZZ_H

# define FUZZ_TIMEOUT_SEC 5
# define FUZZ_ARENA_SIZE (32 * 1024 * 1024) /* arena volontairement énorme :
     on construit ici des AST à la main bien plus gros que ce qu'un vrai
     parser produirait, l'allocateur ne doit pas exploser en plein test */
# define TEST_DIR "/tmp/minishell_executor_fuzz"

# include "../../../include/executor.h"
# include "../../../include/minishell.h"
# include "../../../include/parser.h"
# include "../../../lib/libft/include/ft_stack_alloc.h"
# include "../../../lib/libft/include/ft_hashtable.h"

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <dirent.h>

typedef enum e_fuzz_outcome
{
	FUZZ_OK,        /* revenu dans les temps, pas de crash, pas de leak fd */
	FUZZ_CRASH,     /* mort par signal (segv, abort, bus error, fpe...) */
	FUZZ_TIMEOUT,   /* pas revenu dans les temps -> tué au SIGKILL */
	FUZZ_FD_LEAK,   /* revenu proprement mais a laissé des fd ouverts */
}	t_fuzz_outcome;

typedef struct s_fuzz_result
{
	t_fuzz_outcome	outcome;
	int				exit_code;   /* valide seulement si OK / FD_LEAK */
	int				signal_num;  /* valide seulement si CRASH */
	int				fd_before;
	int				fd_after;
}	t_fuzz_result;

void			init_fuzz_mms(t_mms *mms);
void			free_fuzz_mms(t_mms *mms);

t_tk			*make_token(t_stack_alloc *sa, char *value, t_type_tk type);
t_ast			*make_cmd(t_stack_alloc *sa, char **argv);
t_ast			*make_cmd_raw(t_stack_alloc *sa, t_tk **tokens);
t_ast			*make_pipe(t_stack_alloc *sa, t_ast *left, t_ast *right);
t_ast			*make_and(t_stack_alloc *sa, t_ast *left, t_ast *right);
t_ast			*make_or(t_stack_alloc *sa, t_ast *left, t_ast *right);
t_redir			*make_redir(t_stack_alloc *sa, t_type_tk type, char *file);
void			add_redir(t_ast *ast, t_redir *redir);

t_fuzz_result	run_fuzz_case(t_mms *mms, t_ast *ast);
void			print_fuzz_result(char *name, t_fuzz_result *res);

#endif