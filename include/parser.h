/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 00:32:25 by fiaudfiz         ###   ########.fr       */
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
    t_type_tk    type_tk;
    char            *file;
    struct s_redir  *next;
}   t_redir;

typedef struct s_ast
{
    t_node_type     type;       // type du noeud AST
    struct s_ast    *left;
    struct s_ast    *right;
    t_redir   *redirect;
    union
    {
        t_tk    **tokens;
        //t_ast *subshell;
    };
}   t_ast;

t_ast *parser(t_mms *mms);
t_tk *next_token(t_tk *cur);

#endif