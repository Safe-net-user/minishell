#include "unit.h"
#include <stdarg.h>

void init_test_mms(t_mms *mms)
{
    memset(mms, 0, sizeof(*mms));

    mms->sa = init_stack_allocator(INIT_SIZE_SA);
    mms->cmd_path = init_hash_table(INIT_SIZE_HT);
    mms->alias = init_hash_table(INIT_SIZE_HT); /* était NULL, segfault en attente */

    set_new_value(mms->cmd_path, "PATH", "/usr/bin:/bin");

    mms->env = init_env(INIT_SIZE_HT);
    add_env(mms->env, "PATH", "/usr/bin:/bin", EXPORTED);

    mms->name = "minishell";
    mms->cwd = getcwd(NULL, 0);
    mms->last_status = 0;
}

void free_test_mms(t_mms *mms)
{
    if (mms->cwd)
        free(mms->cwd);
    if (mms->cmd_path)
        free_hash_table(mms->cmd_path);
    if (mms->alias)
        free_hash_table(mms->alias);
    if (mms->env)
        free_env(mms->env);
    if (mms->sa)
        stack_dealloc(mms->sa);
}

t_tk *make_token(t_stack_alloc *sa, char *value, t_type_tk type)
{
    t_tk *token;

    token = stack_alloc(sa, sizeof(t_tk));
    if (!token)
        return (NULL);
    token->value = value;
    token->flags = 0;
    token->type_tk = type;
    return (token);
}

t_ast *make_cmd(t_stack_alloc *sa, char **argv)
{
    t_ast   *ast;
    size_t  i;

    ast = stack_alloc(sa, sizeof(t_ast));
    if (!ast)
        return (NULL);

    ast->type = NODE_CMD;
    ast->left = NULL;
    ast->right = NULL;
    ast->redirect = NULL;

    i = 0;
    while (argv[i])
        i++;

    ast->tokens = stack_alloc(sa, sizeof(t_tk *) * (i + 1));
    if (!ast->tokens)
        return (NULL);

    i = 0;
    while (argv[i])
    {
        ast->tokens[i] = make_token(sa, argv[i], TOK_WORD);
        if (!ast->tokens[i])
            return (NULL);
        i++;
    }
    ast->tokens[i] = NULL;

    return (ast);
}

t_ast *make_pipe(t_stack_alloc *sa, t_ast *left, t_ast *right)
{
    t_ast *ast;

    ast = stack_alloc(sa, sizeof(t_ast));
    if (!ast)
        return (NULL);

    ast->type = NODE_PIPE;
    ast->left = left;
    ast->right = right;
    ast->redirect = NULL;
    ast->tokens = NULL;

    return (ast);
}

t_redir *make_redir(t_stack_alloc *sa,
    t_type_tk type, char *file)
{
    t_redir *redir;

    redir = stack_alloc(sa, sizeof(t_redir));
    if (!redir)
        return (NULL);

    redir->type_tk = type;
    redir->file = file;
    redir->next = NULL;

    return (redir);
}

void add_redir(t_ast *ast, t_redir *redir)
{
    t_redir *current;

    if (!ast->redirect)
    {
        ast->redirect = redir;
        return;
    }

    current = ast->redirect;
    while (current->next)
        current = current->next;
    current->next = redir;
}