/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:51:42 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/25 03:55:38 by fiaudfiz         ###   ########.fr       */
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
		temp = next_token(temp);
	}
	return (count);
}

bool	parse_redirection(t_mms *mms, t_tk **token, t_ast *node)
{
	t_redir	*new;
	t_redir	*cur;

	new = stack_alloc(mms->sa, sizeof(t_redir));
	if (!new)
		return (false);
	new->type_tk = (*token)->type_tk;
	new->next = NULL;
	*token = next_token(*token);
	if (!*token)
		return (parser_error(mms, *token), false);
	if ((*token)->type_tk != TOK_WORD && (*token)->type_tk != TOK_DELIMITER)
		return (parser_error(mms, *token), false);
	new->file = (*token)->value;
	*token = next_token(*token);
	if (!node->redirect)
		node->redirect = new;
	else
	{
		cur = node->redirect;
		while (cur->next)
			cur = cur->next;
		cur->next = new;
	}
	return (true);
}

static void	add_word(t_ast *node, t_tk **token, int *i)
{
	node->tokens[*i] = *token;
	(*i)++;
	*token = next_token(*token);
}

t_ast	*parse_command(t_mms *mms, t_tk **token)
{
	t_ast	*node;
	int		i;
	int		count;

	if (!is_command_token((*token)->type_tk))
		return (parser_error(mms, *token), NULL);
	node = stack_alloc(mms->sa, sizeof(t_ast));
	if (!node)
		return (NULL);
	init_cmd_node(node);
	count = count_tokens(*token);
	node->tokens = stack_alloc(mms->sa,
			sizeof(t_tk *) * (count + 1));
	i = 0;
	while (is_command_token((*token)->type_tk))
	{
		if ((*token)->type_tk == TOK_WORD)
			add_word(node, token, &i);
		else if (!parse_redirection(mms, token, node))
			return (NULL);
	}
	node->tokens[i] = NULL;
	return (node);
}
