/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:51:09 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/23 18:47:14 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parser.c
 * @brief Entry point of the parser and token-list linking.
 *
 * `link_tokens()` walks the raw stack-allocator buffer produced by
 * the lexer, using `next_token()` (offset arithmetic over `t_header`)
 * to rebuild a proper doubly-linked list of tokens up to TOK_EOF.
 *
 * `parser()` links the tokens and hands them to `parse_or_and()`,
 * the highest-priority grammar rule, which recursively builds the
 * final AST.
 *
 * `parser_error()` reports POSIX-style syntax errors to stderr and
 * sets `mms->last_status` to 2, matching bash's convention for
 * shell syntax errors.
 */

#include "parser.h"
#include "minishell.h"
#include <unistd.h>
#include "ft_strings.h"

static void	free_native_token_values(t_tk *first)
{
	t_tk	*tok;

	tok = first;
	while (tok && tok->type_tk != TOK_EOF)
	{
		free(tok->value);
		tok = tok->next;
	}
}

t_tk	*link_tokens(t_tk *token)
{
	t_tk	*temp_next;
	t_tk	*temp_prev;
	t_tk	*head;

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

t_ast	*parser(t_mms *mms)
{
	t_tk	*first;
	t_tk	*head;
	t_ast	*ast;

	first = link_tokens((t_tk *)(mms->sa->buffer + sizeof(t_header)));
	head = first;
	ast = parse_or_and(mms, &first);
	if (!ast)
		free_native_token_values(head);
	return (ast);
}
