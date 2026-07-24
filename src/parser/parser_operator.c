/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:55:04 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 16:38:35 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

/**
 * @brief Converts a token type into its textual representation.
 *
 * Returns the shell syntax associated with the given token type.
 *
 * @param type Token type to convert.
 * @return String representation of the token type.
 */

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

/**
 * @brief Parse a pipeline expression and build PIPE nodes.
 *
 * This function parses commands separated by pipe operators.
 *
 * Example:
 *
 *     ls | grep txt | wc
 *
 * becomes a left-associative AST:
 *
 *          |
 *         / \
 *        |   wc
 *       / \
 *      ls grep
 *
 * Each pipe node stores the previous parsed tree as its left child
 * and the next command as its right child.
 *
 * @param mms Main minishell structure containing the stack allocator.
 * @param token Pointer to the current token pointer.
 *              The pointer advances while parsing.
 *
 * @return Pointer to the root of the generated pipeline AST.
 */

t_ast	*parse_pipe(t_mms *mms, t_tk **token)
{
	t_ast	*left;
	t_ast	*node;

	left = parse_command(mms, token);
	while ((*token)->type_tk == TOK_PIPE)
	{
		node = stack_alloc(mms->sa, sizeof(t_ast));
		node->type = NODE_PIPE;
		node->left = left;
		*token = next_token(*token);
		node->right = parse_command(mms, token);
		if (!node->right)
			return (NULL);
		left = node;
	}
	return (left);
}

/**
 * @brief Parse logical operators (&& and ||).
 *
 * This function parses expressions containing logical operators.
 * It works on top of the pipeline parser to respect shell precedence:
 *
 *     command -> pipeline -> AND/OR
 *
 * Example:
 *
 *     echo a && cat file | grep x
 *
 * becomes:
 *
 *          &&
 *         /  \
 *       echo  |
 *            / \
 *          cat grep
 *
 * @param mms Main minishell structure containing the stack allocator.
 * @param token Pointer to the current token pointer.
 *
 * @return Pointer to the root of the AND/OR AST.
 */

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
		*token = next_token(*token);
		node->right = parse_pipe(mms, token);
		if (!node->right)
			return (NULL);
		left = node;
	}
	return (left);
}
