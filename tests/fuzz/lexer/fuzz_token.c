/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz_tokens.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: you                                           +#+  +:+       +#+     */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24                                  #+#    #+#         */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz_lexer.h"
#include <string.h>

/*
** Each "token piece" is a fragment of minishell grammar. The generator
** picks N pieces at random and concatenates them with a random
** separator (space, tab, nothing) to build syntactically-plausible
** but semantically-abusive command lines.
*/
static const char *g_words[] =
{
	"echo", "cat", "ls", "grep", "wc", "cd", "pwd", "export", "unset",
	"env", "exit", "true", "false", "a", "foo", "bar", "-l", "-n", "--",
	""
};

static const char *g_quoted[] =
{
	"''", "\"\"", "'hello'", "\"hello\"", "'$HOME'", "\"$HOME\"",
	"'it''s'", "\"a\"\"b\"", "'unterminated", "\"unterminated",
	"'$'", "\"$?\"", "'\\n'", "\"\\$HOME\""
};

static const char *g_expansions[] =
{
	"$HOME", "$USER", "$PATH", "$?", "$0", "$", "$$", "$_",
	"${HOME}", "$HOME$USER", "$1", "$99", "$-", "$*", "$@"
};

static const char *g_operators[] =
{
	"|", "||", "<", ">", "<<", ">>", ";", "&", "&&",
	"(", ")", "|||", "<<<", ">>>", "<>", "><"
};

static const char *g_separators[] =
{
	" ", "  ", "\t", "", " \t "
};

static const char	*pick(const char **arr, size_t n)
{
	return (arr[rand() % n]);
}

# define NWORDS   (sizeof(g_words) / sizeof(*g_words))
# define NQUOTED  (sizeof(g_quoted) / sizeof(*g_quoted))
# define NEXPAND  (sizeof(g_expansions) / sizeof(*g_expansions))
# define NOPS     (sizeof(g_operators) / sizeof(*g_operators))
# define NSEPS    (sizeof(g_separators) / sizeof(*g_separators))

/*
** Builds one random token piece into `buf`, never writing more than
** `remaining` bytes (not counting the terminator). Returns the number
** of bytes written so the caller can advance its cursor.
*/
static size_t	next_piece(char *buf, size_t remaining)
{
	const char	*piece;
	int			choice;
	size_t		len;

	choice = rand() % 4;
	if (choice == 0)
		piece = pick(g_words, NWORDS);
	else if (choice == 1)
		piece = pick(g_quoted, NQUOTED);
	else if (choice == 2)
		piece = pick(g_expansions, NEXPAND);
	else
		piece = pick(g_operators, NOPS);
	len = ft_strlen(piece);
	if (len > remaining)
		len = remaining;
	ft_memcpy(buf, piece, len);
	return (len);
}

/*
** Assembles a full random command line out of token pieces joined by
** random separators, capped at MAX_INPUT_SIZE - 1 bytes.
*/
static void	build_token_line(char *buf, size_t cap)
{
	size_t	pos;
	size_t	n_tokens;
	size_t	i;
	const char *sep;
	size_t	seplen;

	pos = 0;
	n_tokens = 1 + (size_t)(rand() % MAX_TOKENS);
	i = 0;
	while (i < n_tokens && pos < cap)
	{
		pos += next_piece(buf + pos, cap - pos);
		if (pos >= cap)
			break ;
		sep = pick(g_separators, NSEPS);
		seplen = ft_strlen(sep);
		if (seplen > cap - pos)
			seplen = cap - pos;
		ft_memcpy(buf + pos, sep, seplen);
		pos += seplen;
		i++;
	}
	buf[pos] = '\0';
}

void	fuzz_phase_tokens(t_mms *mms, const t_fuzz_cfg *cfg, t_fuzz_stats *st)
{
	char	input[MAX_INPUT_SIZE + 1];
	size_t	i;

	printf("== [1] Token-based grammar fuzz (%zu iters) ==\n",
		cfg->token_iters);
	i = 0;
	while (i < cfg->token_iters)
	{
		build_token_line(input, MAX_INPUT_SIZE);
		if (cfg->verbose)
			printf("[tok %zu] \"%s\"\n", i, input);
		else if (i % 50000 == 0)
			printf("[%zu/%zu]\n", i, cfg->token_iters);
		fuzz_run_one(mms, input, ft_strlen(input), st);
		i++;
	}
	printf("\n");
}