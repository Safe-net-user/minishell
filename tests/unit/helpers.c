/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 01:09:06 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 01:09:12 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unit.h"
#include "ft_stack_alloc.h"

t_mms   *make_mms(char **envp)
{
    t_mms *mms;

    mms = calloc(1, sizeof(t_mms));
    mms->env = init_env(INIT_SIZE_HT);
    mms->cmd_path = init_hash_table(INIT_SIZE_HT);
    mms->alias = init_hash_table(INIT_SIZE_HT);
    mms->sa = init_stack_allocator(INIT_SIZE_SA);
    mms->last_status = 0;
    set_exported_env_ht(mms, envp);
    set_var_env_ht(mms);
    return (mms);
}

void    reset_mms(t_mms *mms)
{
    stack_dealloc(mms->sa);
    mms->sa = init_stack_allocator(INIT_SIZE_SA);
}

t_tk    *make_tok(t_mms *mms, t_type_tk type, char *value)
{
    t_tk *tok;

    tok = stack_alloc(mms->sa, sizeof(t_tk));
    tok->type_tk = type;
    tok->value = value;
    tok->flags = 0;
    return (tok);
}

t_ast   *make_cmd(t_mms *mms, char **argv)
{
    t_ast   *node;
    int     i;
    int     argc;

    argc = 0;
    while (argv[argc])
        argc++;
    node = stack_alloc(mms->sa, sizeof(t_ast));
    node->type = NODE_CMD;
    node->left = NULL;
    node->right = NULL;
    node->redirect = NULL;
    node->tokens = stack_alloc(mms->sa, sizeof(t_tk *) * (argc + 1));
    i = 0;
    while (argv[i])
    {
        node->tokens[i] = make_tok(mms, TOK_WORD, argv[i]);
        i++;
    }
    node->tokens[i] = NULL;
    return (node);
}

t_ast   *make_pipe(t_mms *mms, t_ast *left, t_ast *right)
{
    t_ast *node;

    node = stack_alloc(mms->sa, sizeof(t_ast));
    node->type = NODE_PIPE;
    node->left = left;
    node->right = right;
    node->redirect = NULL;
    node->tokens = NULL;
    return (node);
}

t_ast   *make_and(t_mms *mms, t_ast *left, t_ast *right)
{
    t_ast *node;

    node = stack_alloc(mms->sa, sizeof(t_ast));
    node->type = NODE_AND;
    node->left = left;
    node->right = right;
    node->redirect = NULL;
    node->tokens = NULL;
    return (node);
}

t_ast   *make_or(t_mms *mms, t_ast *left, t_ast *right)
{
    t_ast *node;

    node = stack_alloc(mms->sa, sizeof(t_ast));
    node->type = NODE_OR;
    node->left = left;
    node->right = right;
    node->redirect = NULL;
    node->tokens = NULL;
    return (node);
}

t_redir *make_redir(t_mms *mms, t_type_tk type, char *file)
{
    t_redir *r;

    r = stack_alloc(mms->sa, sizeof(t_redir));
    r->type_tk = type;
    r->file = file;
    r->next = NULL;
    return (r);
}