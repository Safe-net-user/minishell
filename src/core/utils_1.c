/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 11:37:49 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/18 14:02:06 by miouali          ###   ########.fr       */
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

int	run_pipeline(t_mms *mms)
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
	if (mms->should_exit)
		return (0);
	return (1);
}
