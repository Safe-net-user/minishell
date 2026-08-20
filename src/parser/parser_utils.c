/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:55:25 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 14:49:32 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"
#include "executor.h"

bool	is_command_token(t_type_tk type)
{
	return (type == TOK_WORD
		|| type == TOK_LESS
		|| type == TOK_DLESS
		|| type == TOK_GREAT
		|| type == TOK_DGREAT
		|| type == TOK_DELIMITER);
}

void	init_cmd_node(t_ast *node)
{
	node->type = NODE_CMD;
	node->left = NULL;
	node->right = NULL;
	node->redirect = NULL;
	node->tokens = NULL;
}

void	add_redirect(t_ast *node, t_tk *new_op)
{
	t_tk	*cur;

	if (!node->redirect)
	{
		node->redirect = new_op;
		return ;
	}
	cur = node->redirect;
	while (cur->next)
		cur = cur->next;
	cur->next = new_op;
	new_op->prev = cur;
}

bool	parse_heredoc(t_mms *mms, t_tk *op, t_tk *file)
{
	char	*content;

	if (op->type_tk != TOK_DLESS)
		return (true);
	content = here_doc(mms, file);
	if (!content)
	{
		if (mms->last_status != 130)
			mms->last_status = 1;
		return (false);
	}
	op->heredoc_content = content;
	return (true);
}

t_tk	*new_redir_token(t_mms *mms, t_tk *src)
{
	t_tk	*new;

	new = stack_alloc(mms->sa, sizeof(t_tk));
	if (!new)
		return (NULL);
	new->type_tk = src->type_tk;
	new->flags = src->flags;
	new->value = src->value;
	new->heredoc_content = NULL;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}
