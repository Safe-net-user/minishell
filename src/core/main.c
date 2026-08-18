/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:30:51 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/18 14:03:27 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include "minishell.h"
#include "ft_memory.h"
#include "ft_string_builder.h"
#include "ft_strings.h"
#include "env.h"
#include "lexer.h"
#include <signal.h>
#include "parser.h"
#include "executor.h"

/**
 * A noter que si HOME est change, alors la ht des commandes doit etre videe.
 */
static	t_mms	*init_og_struct(void)
{
	t_mms	*mms;

	mms = malloc(sizeof(t_mms));
	if (!mms)
		return (0);
	mms->env = init_env(INIT_SIZE_HT);
	mms->alias = init_hash_table(INIT_SIZE_HT);
	mms->cmd_path = init_hash_table(INIT_SIZE_HT);
	mms->sa = init_stack_allocator(INIT_SIZE_SA);
	if (mms->sa)
		ft_bzero(mms->sa->buffer, INIT_SIZE_SA);
	mms->cwd = getcwd(NULL, 0);
	mms->name = ft_strdup("miniMishell");
	mms->last_status = 0;
	mms->umask = 0022;
	mms->should_exit = 0;
	if (!mms->env || !mms->alias || !mms->cmd_path || !mms->sa || !mms->name)
	{
		free_og_struct(mms);
		return (NULL);
	}
	return (mms);
}

static int	set_og_struct(t_mms *mms, char **envp)
{
	if (!mms || !envp)
		return (0);
	if (set_exported_env_ht(mms, envp) == 0)
		return (0);
	if (set_var_env_ht(mms) == 0)
		return (0);
	return (1);
}

static int	handle_input(char *result, t_mms *mms)
{
	if (*result)
		add_history(result);
	if (is_blank(result))
	{
		free(result);
		return (1);
	}
	if (lexer(result, mms) != LX_SUCCESS)
	{
		mms->last_status = 2;
		free(result);
		return (1);
	}
	free(result);
	run_parser_and_exec(mms);
	if (mms->should_exit)
		return (0);
	return (1);
}

static int	process_input(t_mms *mms)
{
	char	*result;

	result = read_line();
	if (!result)
		return (0);
	return (handle_input(result, mms));
}

int	main(int ac, char **av, char **envp)
{
	t_mms	*mms;

	(void)ac;
	(void)av;
	mms = init_og_struct();
	if (!mms || set_og_struct(mms, envp) == 0)
	{
		free_og_struct(mms);
		return (EXIT_FAILURE);
	}
	mms->tty_fd = dup(STDIN_FILENO);
	set_signaux_interactif();
	while (1)
	{
		stack_reset(mms->sa);
		g_signal = 0;
		if (!process_input(mms))
			break ;
	}
	return (cleanup_and_exit(mms));
}
