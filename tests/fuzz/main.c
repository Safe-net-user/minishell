/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 01:11:09 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 01:11:20 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz.h"

static t_mms    *make_mms(char **envp)
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

static void reset_mms(t_mms *mms)
{
    stack_dealloc(mms->sa);
    mms->sa = init_stack_allocator(INIT_SIZE_SA);
}

static t_tk *make_tok(t_mms *mms, t_type_tk type, char *value)
{
    t_tk *tok;

    tok = stack_alloc(mms->sa, sizeof(t_tk));
    tok->type_tk = type;
    tok->value = value;
    tok->flags = 0;
    return (tok);
}

/*
** Cree un NODE_CMD avec entre 1 et 3 args aleatoires
** et optionnellement une redirection
*/
static t_ast    *gen_cmd(t_mms *mms)
{
    t_ast   *node;
    int     argc;
    int     i;

    node = stack_alloc(mms->sa, sizeof(t_ast));
    node->type = NODE_CMD;
    node->left = NULL;
    node->right = NULL;
    node->redirect = NULL;

    argc = rand() % 3 + 1;
    node->tokens = stack_alloc(mms->sa, sizeof(t_tk *) * (argc + 1));
    node->tokens[0] = make_tok(mms, TOK_WORD,
        (char *)safe_cmds[rand() % SAFE_CMDS_COUNT]);
    i = 1;
    while (i < argc)
    {
        node->tokens[i] = make_tok(mms, TOK_WORD,
            (char *)safe_args[rand() % SAFE_ARGS_COUNT]);
        i++;
    }
    node->tokens[i] = NULL;

    /* 30% de chance d'avoir une redirection */
    if (rand() % 10 < 3)
    {
        t_redir *r;
        r = stack_alloc(mms->sa, sizeof(t_redir));
        r->type_tk = redir_types[rand() % REDIR_TYPES_COUNT];
        r->file = (char *)safe_files[rand() % SAFE_FILES_COUNT];
        r->next = NULL;
        node->redirect = r;
    }
    return (node);
}

/*
** Genere un AST aleatoire de profondeur max `depth`
** Peut produire CMD, PIPE, AND, OR
*/
static t_ast    *gen_ast(t_mms *mms, int depth)
{
    int choice;

    if (depth <= 0)
        return (gen_cmd(mms));

    choice = rand() % 4;

    if (choice == 0)
        return (gen_cmd(mms));
    else if (choice == 1)
    {
        t_ast *node = stack_alloc(mms->sa, sizeof(t_ast));
        node->type = NODE_PIPE;
        node->redirect = NULL;
        node->tokens = NULL;
        node->left = gen_ast(mms, depth - 1);
        node->right = gen_cmd(mms);
        return (node);
    }
    else if (choice == 2)
    {
        t_ast *node = stack_alloc(mms->sa, sizeof(t_ast));
        node->type = NODE_AND;
        node->redirect = NULL;
        node->tokens = NULL;
        node->left = gen_ast(mms, depth - 1);
        node->right = gen_ast(mms, depth - 1);
        return (node);
    }
    else
    {
        t_ast *node = stack_alloc(mms->sa, sizeof(t_ast));
        node->type = NODE_OR;
        node->redirect = NULL;
        node->tokens = NULL;
        node->left = gen_ast(mms, depth - 1);
        node->right = gen_ast(mms, depth - 1);
        return (node);
    }
}

static void print_ast_inline(t_ast *node)
{
    if (!node)
        return ;
    if (node->type == NODE_CMD)
    {
        int i = 0;
        while (node->tokens && node->tokens[i])
            printf("%s ", node->tokens[i++]->value);
        if (node->redirect)
            printf("[redir:%d %s] ", node->redirect->type_tk, node->redirect->file);
    }
    else if (node->type == NODE_PIPE)
    {
        print_ast_inline(node->left);
        printf("| ");
        print_ast_inline(node->right);
    }
    else if (node->type == NODE_AND)
    {
        printf("( ");
        print_ast_inline(node->left);
        printf(") && ( ");
        print_ast_inline(node->right);
        printf(") ");
    }
    else if (node->type == NODE_OR)
    {
        printf("( ");
        print_ast_inline(node->left);
        printf(") || ( ");
        print_ast_inline(node->right);
        printf(") ");
    }
}

int main(int ac, char **av, char **envp)
{
    t_mms   *mms;
    t_ast   *ast;
    int     status;
    int     i;
    int     ok;
    int     total;
    int     seed;

    (void)ac;
    (void)av;

    seed = time(NULL);
    srand(seed);
    printf(CYAN "Fuzz seed : %d\n" RESET, seed);

    mms = make_mms(envp);
    ok = 0;
    total = 200;
    i = 0;

    while (i < total)
    {
        int depth = rand() % 3;

        ast = gen_ast(mms, depth);

        printf("\n--------------------------------------------\n");
        printf(YELLOW "FUZZ %03d" RESET " (depth=%d) : ", i + 1, depth);
        print_ast_inline(ast);
        printf("\n");

        /* on redirige stdout/stderr pour eviter le bruit dans le terminal */
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stderr = dup(STDERR_FILENO);
        int devnull = open("/dev/null", 1);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);

        status = executor(mms, ast);

        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stderr, STDERR_FILENO);
        close(saved_stdout);
        close(saved_stderr);

        if (status >= 0 && status <= 255)
        {
            printf(GREEN "PASS (status=%d)\n" RESET, status);
            ok++;
        }
        else
            printf(RED "FAIL (status=%d)\n" RESET, status);

        reset_mms(mms);
        i++;
    }

    printf("\n============================================\n");
    printf(GREEN "PASS : %d / %d\n" RESET, ok, total);
    printf(RED   "FAIL : %d / %d\n" RESET, total - ok, total);
    printf("============================================\n");

    free(mms);
    return (0);
}