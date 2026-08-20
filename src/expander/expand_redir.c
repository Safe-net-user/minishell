/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 12:03:28 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:06:22 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "expander.h"

static bool	is_redir_operator(t_type_tk type)
{
	return (type == TOK_LESS || type == TOK_GREAT
		|| type == TOK_DLESS || type == TOK_DGREAT
		|| type == TOK_LESSAND || type == TOK_GREATAND
		|| type == TOK_LESSGREAT || type == TOK_DLESSDASH
		|| type == TOK_CLOBBER);
}

static t_tk	*advance_after_expand(t_tk **file_ref)
{
	if (*file_ref)
		return ((*file_ref)->next);
	return (NULL);
}

t_val_exp	expand_redirections(t_mms *mms, t_tk **head)
{
	t_tk	*op;
	t_tk	**file_ref;

	if (!mms || !head || !*head)
		return (EXP_SUCCESS);
	op = *head;
	while (op)
	{
		if (!is_redir_operator(op->type_tk) || !op->next)
		{
			op = op->next;
			continue ;
		}
		if (op->type_tk == TOK_DLESS || op->type_tk == TOK_DLESSDASH)
		{
			op = op->next->next;
			continue ;
		}
		file_ref = &op->next;
		if (expand_one(mms, file_ref) != EXP_SUCCESS)
			return (EXP_ERROR);
		op = advance_after_expand(file_ref);
	}
	return (EXP_SUCCESS);
}
