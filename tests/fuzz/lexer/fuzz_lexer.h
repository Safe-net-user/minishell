/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz_lexer.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: you                                           +#+  +:+       +#+     */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24                                  #+#    #+#         */
/*                                                                            */
/* ************************************************************************** */

#ifndef FUZZ_LEXER_H
# define FUZZ_LEXER_H

# include "lexer.h"
# include "minishell.h"
# include "ft_stdlib.h"
# include "ft_strings.h"
# include "ft_memory.h"

# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <stddef.h>
# include <time.h>

# define MAX_INPUT_SIZE   4096
# define MAX_TOKENS       64

/*
** ---- global run configuration (seed, iteration counts) ----
** kept as a single struct so main() can parse argv and pass it
** down to every phase without globals leaking everywhere.
*/
typedef struct s_fuzz_cfg
{
	unsigned int	seed;
	size_t			token_iters;
	size_t			mutate_iters;
	size_t			random_iters;
	int				verbose;
}	t_fuzz_cfg;

/*
** ---- running statistics, printed at the end of each phase ----
*/
typedef struct s_fuzz_stats
{
	size_t	total_runs;
	size_t	crashes;
	size_t	min_len_seen;
	size_t	max_len_seen;
	double	total_time_s;
	double	worst_time_s;
	char	worst_time_input[MAX_INPUT_SIZE + 1];
}	t_fuzz_stats;

/* ---- shared helpers (fuzz_lexer.c) ---- */
t_mms	*fuzz_init_mms(void);
void	fuzz_clear_allocator(t_mms *mms);
void	fuzz_stats_init(t_fuzz_stats *st);
void	fuzz_stats_merge(t_fuzz_stats *dst, const t_fuzz_stats *src);
void	fuzz_stats_print(const char *phase, const t_fuzz_stats *st);
void	fuzz_run_one(t_mms *mms, const char *input, size_t len,
			t_fuzz_stats *st);

/* ---- token-based generator (fuzz_tokens.c) ---- */
void	fuzz_phase_tokens(t_mms *mms, const t_fuzz_cfg *cfg,
			t_fuzz_stats *st);

/* ---- mutation engine (fuzz_mutate.c) ---- */
void	fuzz_phase_mutate(t_mms *mms, const t_fuzz_cfg *cfg,
			t_fuzz_stats *st);

/* ---- static corpus + size boundaries + binary (fuzz_corpus.c) ---- */
void	fuzz_phase_evil(t_mms *mms, t_fuzz_stats *st);
void	fuzz_phase_sizes(t_mms *mms, t_fuzz_stats *st);
void	fuzz_phase_binary(t_mms *mms, const t_fuzz_cfg *cfg,
			t_fuzz_stats *st);

#endif