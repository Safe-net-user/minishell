/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 01:10:48 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 01:10:55 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FUZZ_H
# define FUZZ_H

# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <unistd.h>
# include "executor.h"
# include "minishell.h"
# include "parser.h"
# include "env.h"

# define GREEN  "\033[32m"
# define RED    "\033[31m"
# define CYAN   "\033[36m"
# define YELLOW "\033[33m"
# define RESET  "\033[0m"

# define INIT_SIZE_HT 1024
# define INIT_SIZE_SA 32768

static const char *safe_cmds[] = {
    "true", "false", "echo", "pwd", "ls", "cat", "wc", "env"
};
# define SAFE_CMDS_COUNT 8

static const char *safe_args[] = {
    "hello", "world", "-n", "-l", "-c", "foo", "bar", "42"
};
# define SAFE_ARGS_COUNT 8

static const char *safe_files[] = {
    "/tmp/fuzz_exec_out",
    "/tmp/fuzz_exec_out2",
    "/etc/hostname",
};
# define SAFE_FILES_COUNT 3

static const t_type_tk redir_types[] = {
    TOK_LESS, TOK_GREAT, TOK_DGREAT
};
# define REDIR_TYPES_COUNT 3

#endif