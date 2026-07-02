/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:18 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/01 16:49:29 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

t_token *next_token(t_token *cur)
{
    t_header *h;

    h = (t_header *)cur - 1;
    return ((t_token *)((uint8_t *)cur + h->size + sizeof(t_header)));
}

/**
 * @brief
 * 
 * 
 * 
 * 
 * 
 * @param
 * @param
 */

t_ast   *parse_redirection(t_mms *mms, t_token **token)
{
    
}

/**
 * @brief Parse a simple shell command into an AST node.
 *
 * This function creates a NODE_CMD node and consumes all consecutive
 * WORD tokens. Each word is stored in the argv array of the command.
 *
 * The function stops parsing when it reaches an operator token
 * such as a pipe, AND, OR, or the end of the token stream.
 *
 * @param mms Main minishell structure containing the stack allocator.
 * @param token Pointer to the current token pointer.
 *              The pointer is updated as tokens are consumed.
 *
 * @return Pointer to the created command AST node.
 */


t_ast *parse_command(t_mms *mms, t_token **token)
{
    t_ast   *node;
    t_ast   *left;
    t_ast   *right;
    int     i;

    node = stack_alloc(mms->sa, sizeof(t_ast));
    while ((*token)->type_tk != TOK_WORD)
    {
        if ((*token)->type_tk == TOK_LESS || (*token)->type_tk == TOK_DLESS)
            left = parse_redirection(mms, token);
        if ((*token)->type_tk == TOK_GREAT || (*token)->type_tk == TOK_DGREAT)
            right = parse_redirection(mms, token);
        //*token = next_token(token);
    }
  
    
    node->type = NODE_CMD;
    node->left = NULL; //a changer
    node->right = NULL; //a changer
    node->argv = stack_alloc(mms->sa, sizeof(char *) * 100);
    node->flags = stack_alloc(mms->sa, sizeof(t_flag_token) * 100);
    i = 0;
    while ((*token)->type_tk == TOK_WORD)
    {
        node->argv[i] = (*token)->value;
        node->flags[i] = (*token)->flags;
        *token = next_token(*token);
        i++;
    }
    node->argv[i] = NULL;
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


t_ast *parse_pipe(t_mms *mms, t_token **token)
{
    t_ast   *node;
    t_ast   *left;
    t_ast   *right;

    left = parse_command(mms, token);
    while ((*token)->type_tk == TOK_PIPE)
    {
        node = stack_alloc(mms->sa, sizeof(t_ast));
        node->type = NODE_PIPE;
        node->left = left;
        node->tok_type = (*token)->type_tk;
        node->flags = 0;
        *token = next_token(*token);
        node->right = parse_command(mms, token);
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

t_ast *parse_or_and(t_mms *mms, t_token **token)
{
    t_ast   *node;
    t_ast   *left;
    t_ast   *right;

    left = parse_pipe(mms, token);
    while ((*token)->type_tk == TOK_AND_IF || (*token)->type_tk == TOK_OR_IF)
    {
        node = stack_alloc(mms->sa, sizeof(t_ast));
        if((*token)->type_tk == TOK_AND_IF)
            node->type = NODE_AND;
        else
            node->type = NODE_OR;
        node->left = left;
        node->tok_type = (*token)->type_tk;
        node->flags = 0;
        *token = next_token(*token);
        node->right = parse_pipe(mms, token);
        left = node;
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
    t_token *first;
    t_ast *ast;

    first = (t_token *)(mms->sa->buffer + sizeof(t_header));
    ast = parse_or_and(mms, &first);
    return (ast);
}