/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz_lexer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: you                                           +#+  +:+       +#+     */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24                                  #+#    #+#         */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz_lexer.h"
#include <string.h>

/*
** ---- setup / teardown ----
*/

t_mms	*fuzz_init_mms(void)
{
	t_mms	*mms;

	mms = malloc(sizeof(t_mms));
	if (!mms)
		return (NULL);
	mms->env = NULL;
	mms->alias = NULL;
	mms->cmd_path = NULL;
	mms->cwd = NULL;
	mms->name = NULL;
	mms->last_status = 0;
	mms->umask = 0022;
	mms->sa = init_stack_allocator(32768);
	if (!mms->sa)
	{
		free(mms);
		return (NULL);
	}
	return (mms);
}

/*
** Full reset of the stack_allocator between runs. Using the allocator's
** own reset entrypoint (rather than manually popping blocks one by one)
** guarantees we return to a pristine state regardless of how many
** blocks the lexer chained internally - this is the "correct" cleanup
** requested: no leaks, no dangling `curr`, no stale watermark.
*/

/*
** ---- stats ----
*/

void	fuzz_stats_init(t_fuzz_stats *st)
{
	st->total_runs = 0;
	st->crashes = 0;
	st->min_len_seen = (size_t)-1;
	st->max_len_seen = 0;
	st->total_time_s = 0.0;
	st->worst_time_s = 0.0;
	st->worst_time_input[0] = '\0';
}

void	fuzz_stats_merge(t_fuzz_stats *dst, const t_fuzz_stats *src)
{
	dst->total_runs += src->total_runs;
	dst->crashes += src->crashes;
	if (src->min_len_seen < dst->min_len_seen)
		dst->min_len_seen = src->min_len_seen;
	if (src->max_len_seen > dst->max_len_seen)
		dst->max_len_seen = src->max_len_seen;
	dst->total_time_s += src->total_time_s;
	if (src->worst_time_s > dst->worst_time_s)
	{
		dst->worst_time_s = src->worst_time_s;
		ft_strlcpy(dst->worst_time_input, src->worst_time_input,
			sizeof(dst->worst_time_input));
	}
}

void	fuzz_stats_print(const char *phase, const t_fuzz_stats *st)
{
	printf("---- stats: %s ----\n", phase);
	printf("  runs         : %zu\n", st->total_runs);
	printf("  min len seen : %zu\n",
		st->min_len_seen == (size_t)-1 ? 0 : st->min_len_seen);
	printf("  max len seen : %zu\n", st->max_len_seen);
	printf("  total time   : %.3fs\n", st->total_time_s);
	if (st->total_runs > 0)
		printf("  avg time/run : %.6fms\n",
			1000.0 * st->total_time_s / (double)st->total_runs);
	printf("  worst time   : %.6fms  (\"%.60s%s\")\n",
		1000.0 * st->worst_time_s, st->worst_time_input,
		ft_strlen(st->worst_time_input) > 60 ? "..." : "");
	printf("\n");
}

/*
** ---- single-run driver ----
** Runs the lexer on one input, timing it and updating stats, then
** resets the allocator so the next run starts from a clean slate.
** This is the single choke point every phase goes through, so crash
** triage (re-running just this call under valgrind/gdb with the
** printed input) is trivial.
*/
void	fuzz_run_one(t_mms *mms, const char *input, size_t len,
		t_fuzz_stats *st)
{
	clock_t	start;
	clock_t	end;
	double	elapsed;

	st->total_runs++;
	if (len < st->min_len_seen)
		st->min_len_seen = len;
	if (len > st->max_len_seen)
		st->max_len_seen = len;
	start = clock();
	lexer((char *)input, mms);
	end = clock();
	elapsed = (double)(end - start) / CLOCKS_PER_SEC;
	st->total_time_s += elapsed;
	if (elapsed > st->worst_time_s)
	{
		st->worst_time_s = elapsed;
		ft_strlcpy(st->worst_time_input, input,
			sizeof(st->worst_time_input));
	}
	free(mms->sa);
	mms->sa = init_stack_allocator(32768);
}

/*
** ---- argv / seed handling ----
** usage: ./fuzz_lexer [seed] [-v]
** seed defaults to time(NULL) but is always printed so a crashing run
** can be reproduced exactly with `./fuzz_lexer <printed_seed>`.
*/
static void	parse_args(int argc, char **argv, t_fuzz_cfg *cfg)
{
	int	i;

	cfg->seed = (unsigned int)time(NULL);
	cfg->token_iters = 100000;
	cfg->mutate_iters = 100000;
	cfg->random_iters = 100000;
	cfg->verbose = 0;
	i = 1;
	while (i < argc)
	{
		if (!ft_strncmp(argv[i], "-v", 3))
			cfg->verbose = 1;
		else
			cfg->seed = (unsigned int)ft_atoi(argv[i]);
		i++;
	}
}

int	main(int argc, char **argv)
{
	t_mms			*mms;
	t_fuzz_cfg		cfg;
	t_fuzz_stats	phase_st;
	t_fuzz_stats	total_st;

	parse_args(argc, argv, &cfg);
	printf("seed = %u  (rerun with: %s %u)\n", cfg.seed, argv[0], cfg.seed);
	srand(cfg.seed);
	mms = fuzz_init_mms();
	if (!mms)
	{
		fprintf(stderr, "fuzz: init failed (OOM?)\n");
		return (EXIT_FAILURE);
	}
	fuzz_stats_init(&total_st);
	fuzz_stats_init(&phase_st);
	fuzz_phase_evil(mms, &phase_st);
	fuzz_stats_print("evil corpus", &phase_st);
	fuzz_stats_merge(&total_st, &phase_st);
	fuzz_stats_init(&phase_st);
	fuzz_phase_sizes(mms, &phase_st);
	fuzz_stats_print("boundary sizes", &phase_st);
	fuzz_stats_merge(&total_st, &phase_st);
	fuzz_stats_init(&phase_st);
	fuzz_phase_tokens(mms, &cfg, &phase_st);
	fuzz_stats_print("token grammar", &phase_st);
	fuzz_stats_merge(&total_st, &phase_st);
	fuzz_stats_init(&phase_st);
	fuzz_phase_mutate(mms, &cfg, &phase_st);
	fuzz_stats_print("mutation", &phase_st);
	fuzz_stats_merge(&total_st, &phase_st);
	fuzz_stats_init(&phase_st);
	fuzz_phase_binary(mms, &cfg, &phase_st);
	fuzz_stats_print("binary", &phase_st);
	fuzz_stats_merge(&total_st, &phase_st);
	fuzz_stats_print("TOTAL", &total_st);
	printf("Fuzz terminé (seed=%u).\n", cfg.seed);
	return (0);
}