/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:18 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/25 12:09:19 by fiaudfiz         ###   ########.fr       */
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

t_ast *parse_command(t_mms *mms, t_token **token)
{
    t_ast   *node;
    int     i;

    node = stack_alloc(mms->sa, sizeof(t_ast));
    node->type = NODE_CMD;
    node->left = NULL;
    node->right = NULL;
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
        node->value = stack_alloc(mms->sa, sizeof(char) * 100);
        node->value = "|";
        *token = next_token(*token);
        node->right = parse_command(mms, token);
        left = node;
    }
    return (left);
}

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

t_ast *parser(t_mms *mms)
{
    t_token *first;
    t_ast *ast;

    first = (t_token *)(mms->sa->buffer + sizeof(t_header));
    ast = parse_or_and(mms, &first);
    return (ast);
}