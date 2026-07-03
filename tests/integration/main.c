/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 14:33:39 by miouali           #+#    #+#             */
/*   Updated: 2026/07/03 15:31:34 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "integration.h"

static void print_ast(t_ast *node, int depth)
{
    int     i;
    t_redir *r;

    if (!node)
        return ;
    i = 0;
    while (i++ < depth)
        printf("  ");
    if (node->type == NODE_CMD)
    {
        printf("CMD:");
        i = 0;
        while (node->argv && node->argv[i])
            printf(" [%s]", node->argv[i++]);
        printf("\n");

        if (!node->redirect)
            printf("  redirect = NULL\n");
        else
        {
            printf("  redirect = %p\n", (void *)node->redirect);

            r = node->redirect->in;
            while (r)
            {
                printf("  REDIR_IN: [%s]\n", r->file);
                r = r->next;
            }

            r = node->redirect->out;
            while (r)
            {
                printf("  REDIR_OUT: [%s]\n", r->file);
                r = r->next;
            }
        }
    }
    else
    {
        printf("NODE type=%d\n", node->type);
        print_ast(node->left, depth + 1);
        print_ast(node->right, depth + 1);
        return ;
    }

    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
}

static void run_test(t_mms *mms, const char *title)
{
    t_ast *ast;

    printf("\n=== %s ===\n", title);
    ast = parser(mms);
    if (ast)
        print_ast(ast, 0);
    else
        printf("parser returned NULL\n");
}

static t_stack_alloc *reset_sa(t_mms *mms)
{
    if (mms->sa)
        stack_dealloc(mms->sa);
    mms->sa = init_stack_allocator(32768);
    return (mms->sa);
}

int main(void)
{
    t_mms   *mms;
    t_token *tok;

    mms = calloc(1, sizeof(t_mms));
    mms->sa = init_stack_allocator(32768);

    // Test 1 : ls | cat
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "ls"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_PIPE; tok->value = "|"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "cat"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_EOF; tok->value = NULL; tok->flags = 0;
    run_test(mms, "Test 1 : ls | cat");

    // Test 2 : ls && echo ok
    reset_sa(mms);
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "ls"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_AND_IF; tok->value = "&&"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "echo"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "ok"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_EOF; tok->value = NULL; tok->flags = 0;
    run_test(mms, "Test 2 : ls && echo ok");

    // Test 3 : ls || echo fallback
    reset_sa(mms);
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "ls"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_OR_IF; tok->value = "||"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "echo"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "fallback"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_EOF; tok->value = NULL; tok->flags = 0;
    run_test(mms, "Test 3 : ls || echo fallback");

    // Test 4 : ls | grep foo && echo found || echo nope
    reset_sa(mms);
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "ls"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_PIPE; tok->value = "|"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "grep"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "foo"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_AND_IF; tok->value = "&&"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "echo"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "found"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_OR_IF; tok->value = "||"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "echo"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "nope"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_EOF; tok->value = NULL; tok->flags = 0;
    run_test(mms, "Test 4 : ls | grep foo && echo found || echo nope");

    // Test 5 : cat < infile
    reset_sa(mms);
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "cat"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_LESS; tok->value = "<"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "infile"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_EOF; tok->value = NULL; tok->flags = 0;
    run_test(mms, "Test 5 : cat < infile");

    // Test 6 : echo hello > outfile
    reset_sa(mms);
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "echo"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "hello"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_GREAT; tok->value = ">"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "outfile"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_EOF; tok->value = NULL; tok->flags = 0;
    run_test(mms, "Test 6 : echo hello > outfile");

    // Test 7 : cat < in > out
    reset_sa(mms);
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "cat"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_LESS; tok->value = "<"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "in"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_GREAT; tok->value = ">"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_WORD; tok->value = "out"; tok->flags = 0;
    tok = stack_alloc(mms->sa, sizeof(t_token));
    tok->type_tk = TOK_EOF; tok->value = NULL; tok->flags = 0;
    run_test(mms, "Test 7 : cat < in > out");

    stack_dealloc(mms->sa);
    free(mms);
    return (0);
}