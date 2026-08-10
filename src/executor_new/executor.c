/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/10 12:41:35 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/10 12:43:19 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int	executor(t_mms *mms, t_ast *head)
{
	t_ast	*tmp;
	int		exit_status;

	if (!head)
		return (0);
    //signaux
	tmp = head;
	exit_status = 0;
	if (tmp->type == NODE_CMD)
		exit_status = execute_cmd(mms, tmp);
	else if (tmp->type == NODE_AND)
	{
		exit_status = executor(mms, tmp->left);
		if (exit_status == 0)
			exit_status = executor(mms, tmp->right);
	}
	else if (tmp->type == NODE_OR)
	{
		exit_status = executor(mms, tmp->left);
		if (exit_status != 0)
			exit_status = executor(mms, tmp->right);
	}
	else if (tmp->type == NODE_PIPE)
		exit_status = pipeline(mms, tmp);
	mms->last_status = exit_status;
	return (exit_status);
}