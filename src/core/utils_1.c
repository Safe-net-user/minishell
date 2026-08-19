/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:37:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/19 00:10:30 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

int	get_exit_status(t_mms *mms)
{
	if (mms->should_exit)
		return (mms->exit_status);
	return (mms->last_status);
}

void	run_parser_and_exec(t_mms *mms)
{
	t_ast	*head;

	head = parser(mms);
	if (head)
	{
		mms->current_ast = head;
		mms->last_status = executor(mms, head);
		free_ast_values(head);
		mms->current_ast = NULL;
	}
}
