/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:51:42 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/12 12:50:33 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

t_tk	*next_token(t_tk *cur)
{
	t_header	*h;

	h = (t_header *)cur - 1;
	return ((t_tk *)((uint8_t *)cur + h->size
		+ sizeof(t_header)));
}

int	count_tokens(t_tk *token)
{
	t_tk	*temp;
	int		count;

	temp = token;
	count = 0;
	while (temp && (temp->type_tk == TOK_WORD
			|| temp->type_tk == TOK_LESS
			|| temp->type_tk == TOK_DLESS
			|| temp->type_tk == TOK_GREAT
			|| temp->type_tk == TOK_DGREAT
			|| temp->type_tk == TOK_DELIMITER))
	{
		count++;
		temp = temp->next;
	}
	return (count);
}
//peut etre ajouter prev pour expander donc ici on consomme 2 tokens pour verifier que on a bien un file apres un operator
bool	parse_redirection(t_mms *mms, t_tk *token, t_ast *node)
{
	t_tk	*new_op;
	t_tk	*new_file;
	t_tk	*cur;

	new_op = stack_alloc(mms->sa, sizeof(t_tk));
	if (!new_op)
		return (false);
	new_op->type_tk = token->type_tk;
	new_op->flags = token->flags;
	new_op->value = token->value;
	new_op->next = NULL;
	new_op->prev = NULL;
	token = token->next;
	if (!token)
		return (parser_error(mms, token), false);
	if (token->type_tk != TOK_WORD && token->type_tk != TOK_DELIMITER)
		return (parser_error(mms, token), false);
	new_file = stack_alloc(mms->sa, sizeof(t_tk));
	if (!new_file)
		return (false);
	new_file->type_tk = token->type_tk;
	new_file->flags = token->flags;
	new_file->value = token->value;
	new_file->next = NULL;
	new_file->prev = new_op;
	new_op->next = new_file;
	if (!node->redirect)
		node->redirect = new_op;
	else
	{
		cur = node->redirect;
		while (cur->next)
			cur = cur->next;
		cur->next = new_op;
		new_op->prev = cur;
	}
	return (true);
}
static t_tk	*add_word(t_mms *mms, t_ast *node, t_tk *token)
{
	t_tk	*new_tk;
	t_tk	*cur;

	new_tk = stack_alloc(mms->sa, sizeof(t_tk));
	if (!new_tk)
		return (NULL);
	new_tk->type_tk = token->type_tk;
	new_tk->flags = token->flags;
	new_tk->value = token->value;
	new_tk->next = NULL;
	new_tk->prev = NULL;
	if (!node->tokens)
		node->tokens = new_tk;
	else
	{
		cur = node->tokens;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = new_tk;
		new_tk->prev = cur;
	}
	return (token->next);
}

t_ast	*parse_command(t_mms *mms, t_tk *token)
{
	t_ast	*node;

	if (!is_command_token(token->type_tk))
		return (parser_error(mms, token), NULL);
	node = stack_alloc(mms->sa, sizeof(t_ast));
	if (!node)
		return (NULL);
	init_cmd_node(node);
	while (token && is_command_token(token->type_tk))
	{
		if (token->type_tk == TOK_WORD)
			token = add_word(mms, node, token);
		else
		{
			if (!parse_redirection(mms, token, node))
				return (NULL);
			token = token->next->next;
		}
		if (!token)
			break ;
	}
	return (node);
}
