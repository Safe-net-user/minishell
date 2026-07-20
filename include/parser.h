/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/20 13:20:34 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

#include "lexer.h"
#include "ft_stack_alloc.h"
#include "lexer.h"
#include <stdio.h>

typedef enum e_node_type
{
    NODE_CMD,
    NODE_PIPE,
    NODE_AND,
    NODE_OR,
    NODE_REDIR
} t_node_type;

typedef struct s_redir
{
    t_type_token    type;
    char            *file;
    struct s_redir  *next;
}   t_redir;

typedef struct s_ast
{
    t_node_type     type;       // type du noeud AST
    t_type_token    tok_type;   // type du token lexer (TOK_LESS, TOK_GREAT, etc.)
    t_flag_token    *flags;      // flags du token lexer (SQUOTED, DQUOTED, etc.)
    struct s_ast    *left;
    struct s_ast    *right;
    t_redir   *redirect;
    union
    {
        char            **argv;
        //t_ast *subshell;
    };
}   t_ast;

t_ast *parser(t_mms *mms);
t_token *next_token(t_token *cur);

#endif