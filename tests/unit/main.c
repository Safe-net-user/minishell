/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 01:09:24 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/21 01:09:34 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unit.h"

int main(int ac, char **av, char **envp)
{
    t_mms   *mms;
    t_ast   *ast;
    int     status;

    (void)ac;
    (void)av;
    mms = make_mms(envp);

    /* ============================================================ */
    SECTION("Commandes simples");
    /* ============================================================ */

    /* true retourne 0 */
    ast = make_cmd(mms, (char *[]){"true", NULL});
    status = executor(mms, ast);
    CHECK("true", status, 0);
    reset_mms(mms);

    /* false retourne 1 */
    ast = make_cmd(mms, (char *[]){"false", NULL});
    status = executor(mms, ast);
    CHECK("false", status, 1);
    reset_mms(mms);

    /* commande inexistante -> 127 */
    ast = make_cmd(mms, (char *[]){"cmd_inexistante_42", NULL});
    status = executor(mms, ast);
    CHECK("cmd inexistante (127)", status, 127);
    reset_mms(mms);

    /* echo sans newline */
    ast = make_cmd(mms, (char *[]){"echo", "hello", "world", NULL});
    status = executor(mms, ast);
    CHECK("echo hello world (0)", status, 0);
    reset_mms(mms);

    /* ============================================================ */
    SECTION("Builtins");
    /* ============================================================ */

    /* pwd retourne 0 */
    ast = make_cmd(mms, (char *[]){"pwd", NULL});
    status = executor(mms, ast);
    CHECK("pwd (0)", status, 0);
    reset_mms(mms);

    /* echo -n */
    ast = make_cmd(mms, (char *[]){"echo", "-n", "test", NULL});
    status = executor(mms, ast);
    CHECK("echo -n (0)", status, 0);
    reset_mms(mms);

    /* env retourne 0 */
    ast = make_cmd(mms, (char *[]){"env", NULL});
    status = executor(mms, ast);
    CHECK("env (0)", status, 0);
    reset_mms(mms);

    /* cd vers / */
    ast = make_cmd(mms, (char *[]){"cd", "/", NULL});
    status = executor(mms, ast);
    CHECK("cd / (0)", status, 0);
    reset_mms(mms);

    /* cd vers un dossier inexistant -> 1 */
    ast = make_cmd(mms, (char *[]){"cd", "/dossier_qui_nexiste_pas_42", NULL});
    status = executor(mms, ast);
    CHECK("cd inexistant (1)", status, 1);
    reset_mms(mms);

    /* ============================================================ */
    SECTION("Pipes");
    /* ============================================================ */

    /* echo hello | cat */
    ast = make_pipe(mms,
        make_cmd(mms, (char *[]){"echo", "hello", NULL}),
        make_cmd(mms, (char *[]){"cat", NULL}));
    status = executor(mms, ast);
    CHECK("echo hello | cat (0)", status, 0);
    reset_mms(mms);

    /* ls | wc -l */
    ast = make_pipe(mms,
        make_cmd(mms, (char *[]){"ls", NULL}),
        make_cmd(mms, (char *[]){"wc", "-l", NULL}));
    status = executor(mms, ast);
    CHECK("ls | wc -l (0)", status, 0);
    reset_mms(mms);

    /* echo a | cat | wc -c  (pipeline 3 etages) */
    ast = make_pipe(mms,
        make_pipe(mms,
            make_cmd(mms, (char *[]){"echo", "a", NULL}),
            make_cmd(mms, (char *[]){"cat", NULL})),
        make_cmd(mms, (char *[]){"wc", "-c", NULL}));
    status = executor(mms, ast);
    CHECK("echo a | cat | wc -c (0)", status, 0);
    reset_mms(mms);

    /* ============================================================ */
    SECTION("Operateurs logiques");
    /* ============================================================ */

    /* true && echo ok -> 0 */
    ast = make_and(mms,
        make_cmd(mms, (char *[]){"true", NULL}),
        make_cmd(mms, (char *[]){"echo", "ok", NULL}));
    status = executor(mms, ast);
    CHECK("true && echo ok (0)", status, 0);
    reset_mms(mms);

    /* false && echo ok -> 1 (echo ne s'execute pas) */
    ast = make_and(mms,
        make_cmd(mms, (char *[]){"false", NULL}),
        make_cmd(mms, (char *[]){"echo", "ok", NULL}));
    status = executor(mms, ast);
    CHECK("false && echo ok (1)", status, 1);
    reset_mms(mms);

    /* false || echo fallback -> 0 */
    ast = make_or(mms,
        make_cmd(mms, (char *[]){"false", NULL}),
        make_cmd(mms, (char *[]){"echo", "fallback", NULL}));
    status = executor(mms, ast);
    CHECK("false || echo fallback (0)", status, 0);
    reset_mms(mms);

    /* true || echo pas execute -> 0 */
    ast = make_or(mms,
        make_cmd(mms, (char *[]){"true", NULL}),
        make_cmd(mms, (char *[]){"echo", "pas execute", NULL}));
    status = executor(mms, ast);
    CHECK("true || echo (0)", status, 0);
    reset_mms(mms);

    /* false && echo a || echo b -> 0 */
    ast = make_or(mms,
        make_and(mms,
            make_cmd(mms, (char *[]){"false", NULL}),
            make_cmd(mms, (char *[]){"echo", "a", NULL})),
        make_cmd(mms, (char *[]){"echo", "b", NULL}));
    status = executor(mms, ast);
    CHECK("false && echo a || echo b (0)", status, 0);
    reset_mms(mms);

    /* ============================================================ */
    SECTION("Redirections");
    /* ============================================================ */

    /* echo hello > /tmp/test_exec_out */
    {
        t_ast *cmd;
        cmd = make_cmd(mms, (char *[]){"echo", "hello", NULL});
        cmd->redirect = make_redir(mms, TOK_GREAT, "/tmp/test_exec_out");
        status = executor(mms, cmd);
        CHECK("echo hello > /tmp/test_exec_out (0)", status, 0);
        reset_mms(mms);
    }

    /* cat < /etc/hostname */
    {
        t_ast *cmd;
        cmd = make_cmd(mms, (char *[]){"cat", NULL});
        cmd->redirect = make_redir(mms, TOK_LESS, "/etc/hostname");
        status = executor(mms, cmd);
        CHECK("cat < /etc/hostname (0)", status, 0);
        reset_mms(mms);
    }

    /* cat < /etc/hostname > /tmp/test_exec_copy */
    {
        t_ast   *cmd;
        t_redir *r1;
        t_redir *r2;

        cmd = make_cmd(mms, (char *[]){"cat", NULL});
        r1 = make_redir(mms, TOK_LESS, "/etc/hostname");
        r2 = make_redir(mms, TOK_GREAT, "/tmp/test_exec_copy");
        r1->next = r2;
        cmd->redirect = r1;
        status = executor(mms, cmd);
        CHECK("cat < /etc/hostname > /tmp/test_exec_copy (0)", status, 0);
        reset_mms(mms);
    }

    /* echo append >> /tmp/test_exec_out */
    {
        t_ast *cmd;
        cmd = make_cmd(mms, (char *[]){"echo", "append", NULL});
        cmd->redirect = make_redir(mms, TOK_DGREAT, "/tmp/test_exec_out");
        status = executor(mms, cmd);
        CHECK("echo append >> /tmp/test_exec_out (0)", status, 0);
        reset_mms(mms);
    }

    /* redir vers fichier sans permission -> 1 */
    {
        t_ast *cmd;
        cmd = make_cmd(mms, (char *[]){"echo", "test", NULL});
        cmd->redirect = make_redir(mms, TOK_GREAT, "/proc/no_permission");
        status = executor(mms, cmd);
        CHECK("echo > /proc/no_permission (1)", status, 1);
        reset_mms(mms);
    }

    /* ============================================================ */
    SECTION("Pipes + redirections");
    /* ============================================================ */

    /* cat < /etc/hostname | wc -c */
    {
        t_ast   *left;
        t_ast   *ast2;

        left = make_cmd(mms, (char *[]){"cat", NULL});
        left->redirect = make_redir(mms, TOK_LESS, "/etc/hostname");
        ast2 = make_pipe(mms, left,
            make_cmd(mms, (char *[]){"wc", "-c", NULL}));
        status = executor(mms, ast2);
        CHECK("cat < /etc/hostname | wc -c (0)", status, 0);
        reset_mms(mms);
    }

    /* ============================================================ */
    SECTION("Chemin relatif / absolu");
    /* ============================================================ */

    /* /bin/echo hello */
    ast = make_cmd(mms, (char *[]){"/bin/echo", "hello", NULL});
    status = executor(mms, ast);
    CHECK("/bin/echo hello (0)", status, 0);
    reset_mms(mms);

    /* ./non_existant -> 127 */
    ast = make_cmd(mms, (char *[]){"./non_existant_42", NULL});
    status = executor(mms, ast);
    CHECK("./non_existant (127)", status, 127);
    reset_mms(mms);

    free(mms);
    return (0);
}