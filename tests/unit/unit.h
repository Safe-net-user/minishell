/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unit.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 01:08:45 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 01:08:53 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIT_H
# define UNIT_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include "executor.h"
# include "minishell.h"
# include "parser.h"
# include "lexer.h"
# include "env.h"

# define GREEN  "\033[32m"
# define RED    "\033[31m"
# define CYAN   "\033[36m"
# define YELLOW "\033[33m"
# define RESET  "\033[0m"

# define INIT_SIZE_HT 1024
# define INIT_SIZE_SA 32768

# define CHECK(label, got, expected) do { \
    if ((got) == (expected)) \
        printf(GREEN "  PASS" RESET " %s (status=%d)\n", label, got); \
    else \
        printf(RED   "  FAIL" RESET " %s (got=%d expected=%d)\n", label, got, expected); \
} while (0)

# define SECTION(name) \
    printf("\n" YELLOW "########## %s ##########" RESET "\n", name)

t_mms   *make_mms(char **envp);
void    reset_mms(t_mms *mms);
t_tk    *make_tok(t_mms *mms, t_type_tk type, char *value);
t_ast   *make_cmd(t_mms *mms, char **argv);
t_ast   *make_pipe(t_mms *mms, t_ast *left, t_ast *right);
t_ast   *make_and(t_mms *mms, t_ast *left, t_ast *right);
t_ast   *make_or(t_mms *mms, t_ast *left, t_ast *right);
t_redir *make_redir(t_mms *mms, t_type_tk type, char *file);

#endif