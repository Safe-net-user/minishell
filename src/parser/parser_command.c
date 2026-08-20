/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:51:42 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:01:17 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parser_command.c
 * @brief Parsing of simple commands: words and redirections.
 *
 * `next_token()` computes the address of the token following `cur`
 * directly from its stack-allocator header (`size` field), letting
 * the parser walk the lexer's raw output without an explicit array.
 *
 * `parse_command()` consumes consecutive TOK_WORD and redirection
 * tokens into a single NODE_CMD: words are appended to `node->tokens`
 * via `add_word()`, and redirections are parsed and appended to
 * `node->redirect` via `parse_redirection()` (which also resolves
 * heredocs inline, see `parse_heredoc()`).
 */

#include "parser.h"
#include "executor.h"
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

bool	parse_redirection(t_mms *mms, t_tk **token, t_ast *node)
{
	t_tk	*new_op;
	t_tk	*new_file;

	new_op = new_redir_token(mms, *token);
	if (!new_op)
		return (false);
	*token = (*token)->next;
	if (!*token || ((*token)->type_tk != TOK_WORD
			&& (*token)->type_tk != TOK_DELIMITER))
		return (parser_error(mms, *token), false);
	new_file = new_redir_token(mms, *token);
	if (!new_file)
		return (false);
	new_file->prev = new_op;
	new_op->next = new_file;
	if (!parse_heredoc(mms, new_op, new_file))
		return (false);
	add_redirect(node, new_op);
	*token = (*token)->next;
	return (true);
}

static bool	add_word(t_mms *mms, t_ast *node, t_tk **token)
{
	t_tk	*new_tk;
	t_tk	*cur;

	new_tk = stack_alloc(mms->sa, sizeof(t_tk));
	if (!new_tk)
		return (false);
	new_tk->type_tk = (*token)->type_tk;
	new_tk->flags = (*token)->flags;
	new_tk->value = (*token)->value;
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
	*token = (*token)->next;
	return (true);
}

t_ast	*parse_command(t_mms *mms, t_tk **token)
{
	t_ast	*node;

	if (!is_command_token((*token)->type_tk))
		return (parser_error(mms, *token), NULL);
	node = stack_alloc(mms->sa, sizeof(t_ast));
	if (!node)
		return (NULL);
	init_cmd_node(node);
	while (*token && is_command_token((*token)->type_tk))
	{
		if ((*token)->type_tk == TOK_WORD)
		{
			if (!add_word(mms, node, token))
				return (NULL);
		}
		else if (!parse_redirection(mms, token, node))
			return (NULL);
		if (!*token)
			break ;
	}
	return (node);
}
