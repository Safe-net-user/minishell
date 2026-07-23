/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 13:57:17 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/23 13:57:18 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz.h"

static int	g_total;
static int	g_crash;
static int	g_hang;
static int	g_fdleak;

static void	run_case(t_mms *mms, char *name, t_ast *ast)
{
	t_fuzz_result	res;

	g_total++;
	res = run_fuzz_case(mms, ast);
	print_fuzz_result(name, &res);
	if (res.outcome == FUZZ_CRASH)
		g_crash++;
	else if (res.outcome == FUZZ_TIMEOUT)
		g_hang++;
	else if (res.outcome == FUZZ_FD_LEAK)
		g_fdleak++;
}

static void	group_malformed_ast(t_mms *mms)
{
	t_ast	*ast;
	t_tk	*raw[2];

	run_case(mms, "ast == NULL", NULL);
	ast = make_cmd(mms->sa, NULL);
	run_case(mms, "NODE_CMD avec tokens == NULL", ast);
	{
		char *a[] = {NULL};
		ast = make_cmd(mms->sa, a);
		run_case(mms, "NODE_CMD avec argv vide (tokens[0] == NULL)", ast);
	}
	raw[0] = make_token(mms->sa, NULL, TOK_WORD);
	raw[1] = NULL;
	ast = make_cmd_raw(mms->sa, raw);
	run_case(mms, "NODE_CMD avec tokens[0]->value == NULL", ast);
	{
		char *a[] = {"", NULL};
		ast = make_cmd(mms->sa, a);
		run_case(mms, "NODE_CMD avec tokens[0]->value == \"\"", ast);
	}
	{
		char	*a[] = {"echo", "ok", NULL};
		t_ast	*cmd = make_cmd(mms->sa, a);

		run_case(mms, "NODE_PIPE avec left == NULL",
			make_pipe(mms->sa, NULL, cmd));
		run_case(mms, "NODE_PIPE avec right == NULL",
			make_pipe(mms->sa, cmd, NULL));
		run_case(mms, "NODE_AND avec left == NULL",
			make_and(mms->sa, NULL, cmd));
		run_case(mms, "NODE_OR avec right == NULL",
			make_or(mms->sa, cmd, NULL));
	}
	{
		char *a[] = {"echo", "hi", NULL};
		ast = make_cmd(mms->sa, a);
		add_redir(ast, make_redir(mms->sa, (t_type_tk)9999, "/tmp/fuzz_x"));
		run_case(mms, "redirect avec type_tk invalide (9999)", ast);
	}
	{
		char *a[] = {"echo", "hi", NULL};
		ast = make_cmd(mms->sa, a);
		add_redir(ast, make_redir(mms->sa, TOK_GREAT, NULL));
		run_case(mms, "redirect avec file == NULL", ast);
	}
}

static void	group_filesystem(t_mms *mms)
{
	t_ast	*ast;
	char	long_path[5000];

	{
		char *a[] = {"echo", "hi", NULL};
		ast = make_cmd(mms->sa, a);
		add_redir(ast, make_redir(mms->sa, TOK_GREAT, TEST_DIR));
		run_case(mms, "redirection > vers un dossier (EISDIR)", ast);
	}
	{
		char *a[] = {"cat", NULL};
		ast = make_cmd(mms->sa, a);
		add_redir(ast, make_redir(mms->sa, TOK_LESS,
				TEST_DIR "/inexistant.txt"));
		run_case(mms, "redirection < depuis un fichier absent (ENOENT)", ast);
	}
	{
		char *a[] = {"cat", NULL};
		ast = make_cmd(mms->sa, a);
		add_redir(ast, make_redir(mms->sa, TOK_LESS, TEST_DIR));
		run_case(mms, "redirection < depuis un dossier", ast);
	}
	{
		char *a[] = {"echo", "hi", NULL};
		ast = make_cmd(mms->sa, a);
		add_redir(ast, make_redir(mms->sa, TOK_GREAT,
				"/chemin/qui/nexiste/pas/out.txt"));
		run_case(mms, "redirection >, dossier parent absent (ENOENT)", ast);
	}
	{
		char *a[] = {TEST_DIR, NULL};
		ast = make_cmd(mms->sa, a);
		run_case(mms, "nom de commande == un dossier", ast);
	}
	{
		char	*not_exec = TEST_DIR "/not_executable.txt";
		int		fd;
		char	*a[2];

		fd = open(not_exec, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd >= 0)
		{
			write(fd, "pas un script\n", 14);
			close(fd);
		}
		a[0] = not_exec;
		a[1] = NULL;
		ast = make_cmd(mms->sa, a);
		run_case(mms, "commande == fichier non-executable", ast);
	}
	memset(long_path, 'a', sizeof(long_path) - 1);
	long_path[sizeof(long_path) - 1] = '\0';
	{
		char *a[] = {"echo", "hi", NULL};
		ast = make_cmd(mms->sa, a);
		add_redir(ast, make_redir(mms->sa, TOK_GREAT, long_path));
		run_case(mms, "redirection > avec chemin de 5000 chars (ENAMETOOLONG)", ast);
	}
}

static void	group_extreme(t_mms *mms)
{
	t_ast	*ast;
	char	*huge_arg;
	char	**many_args;
	int		i;

	huge_arg = malloc(5 * 1024 * 1024 + 1);
	if (huge_arg)
	{
		memset(huge_arg, 'x', 5 * 1024 * 1024);
		huge_arg[5 * 1024 * 1024] = '\0';
		{
			char *a[] = {"echo", huge_arg, NULL};
			ast = make_cmd(mms->sa, a);
			run_case(mms, "un seul argument de 5MB", ast);
		}
		free(huge_arg);
	}
	{
		char	*a[] = {"cat", NULL};
		t_ast	*chain;
		int		n;

		chain = make_cmd(mms->sa, a);
		n = 0;
		while (n < 100)
		{
			chain = make_pipe(mms->sa, make_cmd(mms->sa, a), chain);
			n++;
		}
		run_case(mms, "pipeline de 100 etages (cat)", chain);
	}
	{
		char	*a[] = {"true", NULL};
		t_ast	*chain;
		int		n;

		chain = make_cmd(mms->sa, a);
		n = 0;
		while (n < 3000)
		{
			chain = make_pipe(mms->sa, make_cmd(mms->sa, a), chain);
			n++;
		}
		run_case(mms, "arbre de pipe imbrique sur 3000 niveaux (stack overflow ?)", chain);
	}
	many_args = malloc(sizeof(char *) * 100002);
	if (many_args)
	{
		many_args[0] = "echo";
		i = 1;
		while (i < 100001)
			many_args[i++] = "a";
		many_args[100001] = NULL;
		ast = make_cmd(mms->sa, many_args);
		run_case(mms, "commande avec 100000 arguments (E2BIG ?)", ast);
		free(many_args);
	}
	{
		char *a[] = {"echo", "; rm -rf / #", "$(id)", "`whoami`", "|| true", NULL};
		ast = make_cmd(mms->sa, a);
		run_case(mms, "metacaracteres shell passes comme args litteraux", ast);
	}
	{
		char	*left_a[] = {"dd", "if=/dev/zero", "bs=1M", "count=50", NULL};
		char	*right_a[] = {"true", NULL};
		t_ast	*left = make_cmd(mms->sa, left_a);
		t_ast	*right = make_cmd(mms->sa, right_a);

		ast = make_pipe(mms->sa, left, right);
		run_case(mms, "pipe casse: writer vs reader qui quitte tot (SIGPIPE)", ast);
	}
	{
		char	*a[] = {"echo", "beaucoup de redirs", NULL};
		int		n;

		ast = make_cmd(mms->sa, a);
		n = 0;
		while (n < 200)
		{
			add_redir(ast, make_redir(mms->sa, TOK_GREAT,
					TEST_DIR "/many_redirs.txt"));
			n++;
		}
		run_case(mms, "200 redirections chainees sur une commande", ast);
	}
}

int	main(void)
{
	t_mms	mms;

	init_fuzz_mms(&mms);
	mkdir(TEST_DIR, 0755);
	printf("\n============================================================\n");
	printf("        EXECUTOR FUZZ / CRASH-SAFETY SUITE\n");
	printf("  (cas censes echouer; on verifie juste l'absence de\n");
	printf("   crash, de hang et de fuite de file descriptor)\n");
	printf("============================================================\n\n");
	printf("---- AST MALFORME ----\n");
	group_malformed_ast(&mms);
	printf("\n---- FICHIERS / CHEMINS ----\n");
	group_filesystem(&mms);
	printf("\n---- ENTREES EXTREMES ----\n");
	group_extreme(&mms);
	printf("\n============================================================\n");
	printf("TOTAL   : %d\n", g_total);
	printf("CRASH   : %d\n", g_crash);
	printf("HANG    : %d\n", g_hang);
	printf("FD LEAK : %d\n", g_fdleak);
	printf("============================================================\n");
	free_fuzz_mms(&mms);
	return (g_crash != 0 || g_hang != 0);
}