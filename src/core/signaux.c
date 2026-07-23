/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signaux.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:18:50 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/11 21:18:51 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "minishell.h"

int g_signal = 0;

void    handle_sigint(int sig)
{
    g_signal = sig;
    write(STDOUT_FILENO, "\nminiMishell$:", 14);
}

void    set_signaux_interactif(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);
}
