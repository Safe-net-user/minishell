#include "expander.h"
#include "minishell.h"
#include "lexer.h"
#include "env.h"
#include "ft_strings.h"
#include <stdio.h>
#include <stdlib.h>

static t_tk *new_token(char *value)
{
    t_tk *tk;

    tk = malloc(sizeof(*tk));
    if (!tk)
        return (NULL);
    tk->value = ft_strdup(value);
    tk->type_tk = TOK_WORD;
    tk->flags = TOKF_EXPANSION;
    tk->next = NULL;
    tk->prev = NULL;
    return (tk);
}

static void free_tokens(t_tk *tks)
{
    t_tk *tmp;

    while (tks)
    {
        tmp = tks->next;
        free(tks->value);
        free(tks);
        tks = tmp;
    }
}

static t_tk *make_tokens(char *str)
{
    return (new_token(str));
}

static int run_test(t_mms *mms, char *input, char *expected)
{
    t_tk *head;

    head = make_tokens(input);

    if (expand_tokens(mms, &head) != EXP_SUCCESS)
    {
        printf("[KO] expand_tokens() failed\n");
        free_tokens(head);
        return (0);
    }

    if (!head)
    {
        if (!ft_strcmp("", expected))
            printf("[OK] \"%s\" -> \"%s\"\n", input, expected);
        else
        {
            printf("[KO]\n");
            printf("input    : %s\n", input);
            printf("expected : %s\n", expected);
            printf("got      : (token supprime, valeur vide)\n");
        }
        return (1);
    }

    if (!ft_strcmp(head->value, expected))
        printf("[OK] \"%s\" -> \"%s\"\n", input, expected);
    else
    {
        printf("[KO]\n");
        printf("input    : %s\n", input);
        printf("expected : %s\n", expected);
        printf("got      : %s\n", head->value);
    }

    free_tokens(head);
    return (1);
}

static	t_mms	*init_og_struct(void)
{
    t_mms	*mms;

    mms = malloc(sizeof(t_mms));
    if (!mms)
        return (0);
    mms->env = init_env(INIT_SIZE_HT);
    mms->alias = NULL;
    mms->cmd_path = NULL;
    mms->sa = NULL;
    mms->cwd = NULL;
    mms->name = NULL;
    mms->last_status = 0;
    return (mms);
}


int	main(void)
{
	t_mms	*mms;

	mms = init_og_struct();
	add_env(mms->env, "HOME", "/home/gaspard", 0);
	add_env(mms->env, "USER", "gaspard", 0);
	add_env(mms->env, "EMPTY", "", 0);

	/* --- cas de base (déjà présents) --- */
	run_test(mms, "$HOME", "/home/gaspard");
	run_test(mms, "\"$HOME\"", "/home/gaspard");
	run_test(mms, "'$HOME'", "$HOME");
	run_test(mms, "abc$HOME", "abc/home/gaspard");
	run_test(mms, "$HOME/test", "/home/gaspard/test");
	run_test(mms, "\"$HOME/test\"", "/home/gaspard/test");

	/* --- variable suivie d'un délimiteur non-alnum --- */
	run_test(mms, "$HOME:$USER", "/home/gaspard:gaspard");
	run_test(mms, "$HOME-test", "/home/gaspard-test");
	run_test(mms, "$HOME.$USER", "/home/gaspard.gaspard");
	run_test(mms, "$HOME$USER", "/home/gaspardgaspard");

	/* --- variable inexistante --- */
	run_test(mms, "$NOPE", "");
	run_test(mms, "$NOPE/test", "/test");
	run_test(mms, "abc$NOPE", "abc");

	/* --- variable vide --- */
	run_test(mms, "$EMPTY", "");
	run_test(mms, "$EMPTYabc", "");
	run_test(mms, "a$EMPTYb", "a");

	/* --- $ seul ou suivi d'un caractère invalide --- */
	run_test(mms, "$", "$");
	run_test(mms, "$ ", "$ ");
	run_test(mms, "$$", "$$");
	run_test(mms, "a$", "a$");
	run_test(mms, "$/test", "$/test");
	run_test(mms, "$-", "$-");

	/* --- $? (exit status) --- */
	run_test(mms, "$?", "0");
	run_test(mms, "exit:$?", "exit:0");
	run_test(mms, "$?$?", "00");

	/* --- guillemets simples : tout est littéral --- */
	run_test(mms, "'abc$HOME'", "abc$HOME");
	run_test(mms, "'$NOPE'", "$NOPE");
	run_test(mms, "'  spaces  '", "  spaces  ");
	run_test(mms, "''", "");

	/* --- guillemets doubles : expansion active, espaces gardés --- */
	run_test(mms, "\"a b c\"", "a b c");
	run_test(mms, "\"$HOME $USER\"", "/home/gaspard gaspard");
	run_test(mms, "\"\"", "");
	run_test(mms, "\"$NOPE\"", "");

	/* --- guillemets adjacents / mélangés --- */
	run_test(mms, "\"$HOME\"'$USER'", "/home/gaspard$USER");
	run_test(mms, "'$HOME'\"$USER\"", "$HOMEgaspard");
	run_test(mms, "\"abc\"'def'", "abcdef");
	run_test(mms, "$HOME\" \"$USER", "/home/gaspard gaspard");

	/* --- underscore et chiffres dans les noms de var --- */
	add_env(mms->env, "MY_VAR_1", "value1", 0);
	run_test(mms, "$MY_VAR_1", "value1");
	run_test(mms, "$MY_VAR_1_", "");

	/* --- fin de string brutale (pas de \0 oublié) --- */
	run_test(mms, "$HOME", "/home/gaspard");
	run_test(mms, "test$HOME", "test/home/gaspard");

	return (0);
}