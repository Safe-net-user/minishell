/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz_mutate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: you                                           +#+  +:+       +#+     */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24                                  #+#    #+#         */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz_lexer.h"
#include <string.h>

/* seed corpus that mutations are applied on top of */
static const char *g_seeds[] =
{
	"echo hello world",
	"cat file.txt | grep foo | wc -l",
	"cat << EOF\nsome text\nEOF",
	"echo \"$HOME/$USER\" > out.txt",
	"ls -la; echo $?; export X=1",
	"echo 'single $HOME quote' \"double $HOME quote\"",
	"cmd1 | cmd2 | cmd3 | cmd4 | cmd5",
	"echo a && echo b || echo c",
	"echo $$$$ $? $0 $-",
	"",
};

# define NSEEDS (sizeof(g_seeds) / sizeof(*g_seeds))

static const char g_mutation_bytes[] =
	"$\"'\\|<>&;()`\t\n \x01\xff";

typedef enum e_mut_kind
{
	MUT_INSERT,
	MUT_DELETE,
	MUT_REPLACE,
	MUT_DUPLICATE,
	MUT_TRUNCATE,
	MUT_KIND_COUNT
}	t_mut_kind;

/*
** Insert a random byte at a random position.
*/
static size_t	mut_insert(char *buf, size_t len, size_t cap)
{
	size_t	pos;
	char	c;

	if (len >= cap)
		return (len);
	pos = (size_t)rand() % (len + 1);
	c = g_mutation_bytes[rand() % (sizeof(g_mutation_bytes) - 1)];
	memmove(buf + pos + 1, buf + pos, len - pos);
	buf[pos] = c;
	return (len + 1);
}

/*
** Delete one byte at a random position.
*/
static size_t	mut_delete(char *buf, size_t len)
{
	size_t	pos;

	if (len == 0)
		return (len);
	pos = (size_t)rand() % len;
	memmove(buf + pos, buf + pos + 1, len - pos - 1);
	return (len - 1);
}

/*
** Replace one byte at a random position with a random byte.
*/
static size_t	mut_replace(char *buf, size_t len)
{
	size_t	pos;

	if (len == 0)
		return (len);
	pos = (size_t)rand() % len;
	buf[pos] = g_mutation_bytes[rand() % (sizeof(g_mutation_bytes) - 1)];
	return (len);
}

/*
** Duplicate a random slice of the buffer, appended right after itself
** (classic "double the delimiter" style bug hunt: quotes, pipes...).
*/
static size_t	mut_duplicate(char *buf, size_t len, size_t cap)
{
	size_t	start;
	size_t	slice;
	size_t	room;

	if (len == 0 || len >= cap)
		return (len);
	start = (size_t)rand() % len;
	slice = 1 + (size_t)rand() % (len - start);
	room = cap - len;
	if (slice > room)
		slice = room;
	memmove(buf + start + slice, buf + start, len - start);
	ft_memcpy(buf + start, buf + start, slice);
	return (len + slice);
}

/*
** Truncate the buffer at a random position (including 0, i.e. empty).
*/
static size_t	mut_truncate(size_t len)
{
	if (len == 0)
		return (0);
	return ((size_t)rand() % len);
}

/*
** Applies one randomly-chosen mutation kind to buf/len in place.
** Returns the new length.
*/
static size_t	apply_mutation(char *buf, size_t len, size_t cap)
{
	int	kind;

	kind = rand() % MUT_KIND_COUNT;
	if (kind == MUT_INSERT)
		return (mut_insert(buf, len, cap));
	if (kind == MUT_DELETE)
		return (mut_delete(buf, len));
	if (kind == MUT_REPLACE)
		return (mut_replace(buf, len));
	if (kind == MUT_DUPLICATE)
		return (mut_duplicate(buf, len, cap));
	return (mut_truncate(len));
}

void	fuzz_phase_mutate(t_mms *mms, const t_fuzz_cfg *cfg, t_fuzz_stats *st)
{
	char	input[MAX_INPUT_SIZE + 1];
	size_t	len;
	size_t	i;
	size_t	n_muts;
	size_t	m;

	printf("== [2] Mutation-based fuzz (%zu iters) ==\n", cfg->mutate_iters);
	i = 0;
	while (i < cfg->mutate_iters)
	{
		ft_strlcpy(input, g_seeds[rand() % NSEEDS], sizeof(input));
		len = ft_strlen(input);
		n_muts = 1 + (size_t)(rand() % 8);
		m = 0;
		while (m < n_muts)
		{
			len = apply_mutation(input, len, MAX_INPUT_SIZE);
			m++;
		}
		input[len] = '\0';
		if (cfg->verbose)
			printf("[mut %zu] (%zu muts) \"%s\"\n", i, n_muts, input);
		else if (i % 50000 == 0)
			printf("[%zu/%zu]\n", i, cfg->mutate_iters);
		fuzz_run_one(mms, input, len, st);
		i++;
	}
	printf("\n");
}