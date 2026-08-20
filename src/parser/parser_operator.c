/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:55:04 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 16:01:27 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parser_operator.c
 * @brief Parsing of pipes and logical operators (&&, ||).
 *
 * Implements two levels of the grammar, each left-associative:
 *
 *     parse_or_and  → parse_pipe (AND_IF | OR_IF) parse_pipe ...
 *     parse_pipe    → parse_command (PIPE parse_command)...
 *
 * `parse_or_and()` sits above `parse_pipe()` in precedence, so pipes
 * bind tighter than `&&`/`||`, matching standard shell grammar.
 * `token_to_string()` provides readable operator names for syntax
 * error messages.
 */

#include "parser.h"
#include "minishell.h"

char	*token_to_string(t_type_tk type)
{
	if (type == TOK_PIPE)
		return ("|");
	if (type == TOK_AND_IF)
		return ("&&");
	if (type == TOK_OR_IF)
		return ("||");
	if (type == TOK_LESS)
		return ("<");
	if (type == TOK_GREAT)
		return (">");
	if (type == TOK_DLESS)
		return ("<<");
	if (type == TOK_DGREAT)
		return (">>");
	if (type == TOK_EOF)
		return ("newline");
	return ("token");
}

t_ast	*parse_pipe(t_mms *mms, t_tk **token)
{
	t_ast	*left;
	t_ast	*node;

	left = parse_command(mms, token);
	if (!left)
		return (NULL);
	while (*token && (*token)->type_tk == TOK_PIPE)
	{
		node = stack_alloc(mms->sa, sizeof(t_ast));
		if (!node)
			return (NULL);
		node->type = NODE_PIPE;
		node->left = left;
		*token = (*token)->next;
		if (!*token)
			return (NULL);
		node->right = parse_command(mms, token);
		if (!node->right)
			return (NULL);
		left = node;
	}
	return (left);
}

t_ast	*parse_or_and(t_mms *mms, t_tk **token)
{
	t_ast	*left;
	t_ast	*node;

	left = parse_pipe(mms, token);
	if (!left)
		return (NULL);
	while (*token && ((*token)->type_tk == TOK_AND_IF
			|| (*token)->type_tk == TOK_OR_IF))
	{
		node = stack_alloc(mms->sa, sizeof(t_ast));
		if (!node)
			return (NULL);
		if ((*token)->type_tk == TOK_AND_IF)
			node->type = NODE_AND;
		else
			node->type = NODE_OR;
		node->left = left;
		*token = (*token)->next;
		node->right = parse_pipe(mms, token);
		if (!node->right)
			return (NULL);
		left = node;
	}
	return (left);
}
