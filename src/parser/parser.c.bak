/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:51:09 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 13:18:25 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

/**
 * @brief Print a parser syntax error.
 *
 * Displays an error message compatible with the shell syntax error format.
 * If the unexpected token is the end of the input stream, "newline" is
 * printed instead.
 *
 * @param mms Main minishell structure.
 * @param tok Unexpected token that caused the syntax error.
 */

void	parser_error(t_mms *mms, t_tk *tok)
{
	char	*token;

	mms->last_status = 2;
	if (tok->type_tk == TOK_EOF)
	{
		fprintf(stderr,
			"minishell: syntax error near unexpected token `newline'\n");
		return ;
	}
	token = tok->value;
	if (!token)
		token = token_to_string(tok->type_tk);
	fprintf(stderr,
		"minishell: syntax error near unexpected token `%s'\n",
		token);
}

/**
 * @brief Entry point of the shell parser.
 *
 * Initializes the token iterator from the stack allocator buffer
 * and starts parsing from the highest grammar level.
 *
 * Parsing order:
 *
 *     COMMAND
 *        |
 *      PIPE
 *        |
 *      AND / OR
 *
 * The returned AST represents the complete shell command structure.
 *
 * @param mms Main minishell structure.
 *
 * @return Pointer to the root AST node.
 */

t_ast	*parser(t_mms *mms)
{
	t_tk	*first;

	first = (t_tk *)(mms->sa->buffer + sizeof(t_header));
	return (parse_or_and(mms, &first));
}
