/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:51:42 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/24 16:39:05 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

/**
 * @brief Returns the token located immediately after the current token.
 *
 * Retrieves the allocation header preceding the current token, then uses
 * the stored block size to compute the address of the next token.
 *
 * @param cur Current token.
 * @return Pointer to the next token in memory.

 */

t_tk	*next_token(t_tk *cur)
{
	t_header	*h;

	h = (t_header *)cur - 1;
	return ((t_tk *)((uint8_t *)cur + h->size
		+ sizeof(t_header)));
}

/**
 * @brief Counts the consecutive command-related tokens.
 *
 * Traverses the token sequence starting at the given token and counts
 * word and redirection tokens until another token type is encountered.
 *
 * @param token First token to examine.
 * @return Number of consecutive command-related tokens.
 */

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

/**
 * @brief Parse a redirection operator.
 *
 * Dispatches the parsing to either the input or output redirection parser
 * depending on the current token.
 *
 * Supported operators:
 *
 *     <   <<
 *     >   >>
 *
 * @param mms Main minishell structure.
 * @param token Pointer to the current token pointer.
 * @param node Command node receiving the parsed redirection.
 *
 * @return true on success, false if a syntax error occurs.
 */

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

/**
 * @brief Parse a simple command and its redirections.
 *
 * This function creates a NODE_CMD node and consumes all consecutive
 * command words and redirection operators.
 *
 * Every WORD token is stored in the argv array, while redirection
 * operators are attached to the command redirection lists.
 *
 * Parsing stops when an operator with a lower precedence (|, &&, ||)
 * or the end of the input is reached.
 *
 * @param mms Main minishell structure containing the stack allocator.
 * @param token Pointer to the current token pointer.
 *              The pointer is updated as tokens are consumed.
 *
 * @return Pointer to the created command AST node, or NULL if a syntax
 *         error is encountered.
 */

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
	node->type = NODE_CMD;
	node->left = NULL;
	node->right = NULL;
	node->redirect = NULL;
	count = count_tokens(*token);
	node->tokens = stack_alloc(mms->sa,
			sizeof(t_tk *) * (count + 1));
	if (!node->tokens) {
		free(node);
		return (NULL);
	}
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
