/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:51:09 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/12 13:13:26 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"
#include <unistd.h>
#include "ft_strings.h"

t_tk *link_tokens(t_tk *token)
{
	t_tk *temp_next;
	t_tk *temp_prev;
	t_tk *head;

	head = token;
	temp_prev = NULL;
	while (token->type_tk != TOK_EOF)
	{
		token->prev = temp_prev;
		temp_prev = token;
		temp_next = next_token(token);
		token->next = temp_next;
		token = next_token(token);
	}
	token->prev = temp_prev;
	token->next = NULL;
	return (head);
}

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
		write(2,
			"minishell: syntax error near unexpected token `newline'\n",
			57);
		return ;
	}
	token = tok->value;
	if (!token)
		token = token_to_string(tok->type_tk);
	write(2, "minishell: syntax error near unexpected token `", 48);
	write(2, token, ft_strlen(token));
	write(2, "'\n", 2);
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

	first = link_tokens((t_tk *)(mms->sa->buffer + sizeof(t_header)));
	return (parse_or_and(mms, first));
}
