/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:18 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 00:19:34 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

t_tk *next_token(t_tk *cur)
{
    t_header *h;

    h = (t_header *)cur - 1;
    return ((t_tk *)((uint8_t *)cur + h->size + sizeof(t_header)));
}

int count_tokens(t_tk *token)
{
    t_tk *temp;
	int	count;

    temp = token;
	count = 0;
	while (temp->type_tk == TOK_WORD
		|| temp->type_tk == TOK_LESS
		|| temp->type_tk == TOK_DLESS
		|| temp->type_tk == TOK_GREAT
		|| temp->type_tk == TOK_DGREAT)
	{
		count++;
		temp = next_token(temp);
	}
	return(count);
}

char *token_to_string(t_type_tk type)
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
	(void)mms;
	if (tok->type_tk == TOK_EOF)
		fprintf(stderr,
			"minishell: syntax error near unexpected token `newline'\n");
	else
		fprintf(stderr,
			"minishell: syntax error near unexpected token `%s'\n",
			tok->value ? tok->value : token_to_string(tok->type_tk));
	//g_signal = 2;
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
	t_redir	*cur;
	t_redir	*new;

	new = stack_alloc(mms->sa, sizeof(t_redir));
	new->type_tk = (*token)->type_tk;
	new->next = NULL;
	*token = next_token(*token);
	if ((*token)->type_tk != TOK_WORD)
	{
		parser_error(mms, *token);
		return (false);
	}
	new->file = (*token)->value;
	*token = next_token(*token);
	if (!node->redirect)
	{
		node->redirect = new;
		return (true);
	}
	cur = node->redirect;
	while (cur->next)
		cur = cur->next;
	cur->next = new;
	return (true);
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
	int 	count_tk;

	if ((*token)->type_tk != TOK_WORD
		&& (*token)->type_tk != TOK_LESS
		&& (*token)->type_tk != TOK_DLESS
		&& (*token)->type_tk != TOK_GREAT
		&& (*token)->type_tk != TOK_DGREAT)
	{
		parser_error(mms, *token);
		return (NULL);
	}
	node = stack_alloc(mms->sa, sizeof(t_ast));
	node->redirect = stack_alloc(mms->sa, sizeof(t_redir));
	node->redirect = NULL;
	node->type = NODE_CMD;
	node->left = NULL;
	node->right = NULL;
	count_tk = count_tokens(*token);
	node->tokens = stack_alloc(mms->sa, sizeof(t_tk) * count_tk + 1);
	i = 0;
	while ((*token)->type_tk == TOK_WORD
		|| (*token)->type_tk == TOK_LESS
		|| (*token)->type_tk == TOK_DLESS
		|| (*token)->type_tk == TOK_GREAT
		|| (*token)->type_tk == TOK_DGREAT)
	{
		if ((*token)->type_tk == TOK_WORD)
		{
			node->tokens[i] = (*token); //jsp si ca suffit
			*token = next_token(*token);
			i++;
		}
		else
		{
			if (!parse_redirection(mms, token, node))
				return (NULL);
		}
	}
	node->tokens[i] = NULL;
	return (node);
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


t_ast *parse_pipe(t_mms *mms, t_tk **token)
{
    t_ast   *node;
    t_ast   *left;

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

t_ast *parse_or_and(t_mms *mms, t_tk **token)
{
    t_ast   *node;
    t_ast   *left;

    left = parse_pipe(mms, token);
    while ((*token)->type_tk == TOK_AND_IF || (*token)->type_tk == TOK_OR_IF)
    {
        node = stack_alloc(mms->sa, sizeof(t_ast));
        if((*token)->type_tk == TOK_AND_IF)
            node->type = NODE_AND;
        else
            node->type = NODE_OR;
        node->left = left;
        *token = next_token(*token);
        node->right = parse_pipe(mms, token);
        if (!node->right)
            return (NULL);
;        left = node;
    }
    return (left);
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

t_ast *parser(t_mms *mms)
{
    t_tk *first;
    t_ast *ast;

    first = (t_tk *)(mms->sa->buffer + sizeof(t_header));
    ast = parse_or_and(mms, &first);
    return (ast);
}