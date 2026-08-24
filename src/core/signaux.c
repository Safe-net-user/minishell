/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signaux.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:18:50 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 17:26:56 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file signaux.c
 * @brief Signal handling for the shell's interactive and heredoc contexts.
 *
 * Two distinct signal configurations are used depending on where the
 * shell is in its execution:
 *
 * - `set_signaux_interactif()`: active while waiting for input at the
 *   prompt. SIGINT redraws a fresh prompt line instead of killing the
 *   shell (mimics bash behavior on Ctrl+C); SIGQUIT is ignored.
 *
 * - `set_signaux_heredoc()`: active while reading heredoc input.
 *   SIGINT sets the global flag and lets the heredoc reading loop
 *   detect and abort itself; SIGQUIT is ignored.
 *
 * `g_signal` is the sole global variable, as required by the subject:
 * it only carries the last received signal number so the rest of the
 * shell can react to it outside the handler.
 */

#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "minishell.h"
#include "readline/readline.h"

int	g_signal = 0;

static void	handle_sigint(int signum)
{
	(void)signum;
	if (write(STDOUT_FILENO, "\n", 1) == -1)
		return ;
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_signal = SIGINT;
}

void	set_signaux_interactif(void)
{
	struct sigaction	sa;

	g_signal = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_sigint;
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

static void	handle_sigint_heredoc(int signum)
{
	(void)signum;
	g_signal = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
}

void	set_signaux_heredoc(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handle_sigint_heredoc;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}
