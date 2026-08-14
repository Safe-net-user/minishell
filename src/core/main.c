/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:30:51 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/14 21:40:17 by fiaudfiz         ###   ########.fr       */
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
 * Penser a revenir sur cette taille car on risque largement de cache miss.
 * (n x sizeof(type))
 * Penser aux conditions si une allocation rate.
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
		ft_bzero(mms->sa->buffer, INIT_SIZE_SA); // zero-init sans toucher au submodule
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
	t_ast	*head;

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
	head = parser(mms);
	if (head)
	{
		mms->current_ast = head;
		mms->last_status = executor(mms, head);
		free_ast_values(head);
		mms->current_ast = NULL;
	}
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
	int		exit_status;

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
	exit_status = mms->should_exit ? mms->exit_status : mms->last_status; //ternaire interdit
	close(mms->tty_fd);
	free_og_struct(mms);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	return (exit_status);
}
