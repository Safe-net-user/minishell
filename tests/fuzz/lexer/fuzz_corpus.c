/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzz_corpus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: you                                           +#+  +:+       +#+     */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24                                  #+#    #+#         */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz_lexer.h"
#include <string.h>

static const char *g_evil[] =
{
	"",
	"'",
	"\"",
	"''",
	"\"\"",
	"''''''''''''''''''''''''''''''''''''''''''''''''''''''''''",
	"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
	"$",
	"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
	"${{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{",
	"\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\",
	"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<",
	">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",
	"<<<<<<<<<<>>>>>>>>>>>>><<<<<<<<<<<<<<<",
	"||||||||||||||||||||||||||||||||||||||",
	"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&",
	";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;",
	"(((((((((((((((((((((((((((((((((((",
	"))))))))))))))))))))))))))))))))))))",
	"`",
	"``````````````````````````````",
	"echo \"",
	"echo '",
	"echo \\",
	"echo $?",
	"echo $$$$$$$$$$$$$$$$$$$$$$$$$$$$",
	"echo <<<<<<<<<<<<<<<<<<<<<<<<<<<<<",
	"echo >>>>>>>>>>>>>>>>>>>>>>>>>>>>>",
	"cat << EOF",
	"$((",
	"$(((",
	"$(((((((((((((((((((((",
	"\"'\"'\"'\"'\"'\"'\"'\"'\"",
	"'\"'\"'\"'\"'\"'\"'\"'",
	" ",
	"\t",
	"\n",
	"   \t\t\t\n\n\n   ",
	"echo\\ hi",
	"e\\c\\h\\o hi",
	"echo$HOME",
	"\"$\"",
	"'$'",
	"$'",
	"$\"",
	NULL
};

void	fuzz_phase_evil(t_mms *mms, t_fuzz_stats *st)
{
	int	i;

	printf("== [3] Static evil corpus ==\n");
	i = 0;
	while (g_evil[i])
	{
		printf("[evil %d] \"%s\"\n", i, g_evil[i]);
		fuzz_run_one(mms, g_evil[i], ft_strlen(g_evil[i]), st);
		i++;
	}
	printf("\n");
}

/*
** Fills buf with `len` repetitions of `c` (no NUL inserted mid-buffer)
** and NUL-terminates at len.
*/
static void	fill_repeat(char *buf, size_t len, char c)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		buf[i] = c;
		i++;
	}
	buf[len] = '\0';
}

/*
** Boundary sizes: 0, 1, 2, small primes/edges, powers of two minus/plus
** one, and MAX_INPUT_SIZE itself. Each size is tried with a few
** different fill characters that are known lexer pain points.
*/
void	fuzz_phase_sizes(t_mms *mms, t_fuzz_stats *st)
{
	static const size_t	sizes[] =
	{
		0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 32, 33,
		63, 64, 65, 127, 128, 129, 255, 256, 257, 511, 512,
		513, 1023, 1024, 1025, 2047, 2048, 2049, 4095, 4096
	};
	static const char		fills[] = { 'a', ' ', '\'', '"', '$', '|', '\\' };
	char					buf[MAX_INPUT_SIZE + 1];
	size_t					si;
	size_t					fi;
	size_t					len;

	printf("== [4] Boundary size sweep (0..%d) ==\n", MAX_INPUT_SIZE);
	si = 0;
	while (si < sizeof(sizes) / sizeof(*sizes))
	{
		len = sizes[si];
		if (len > MAX_INPUT_SIZE)
			len = MAX_INPUT_SIZE;
		fi = 0;
		while (fi < sizeof(fills))
		{
			fill_repeat(buf, len, fills[fi]);
			fuzz_run_one(mms, buf, len, st);
			fi++;
		}
		printf("[size %zu] x%zu fills ok\n", len, sizeof(fills));
		si++;
	}
	printf("\n");
}

/*
** Fills buf with `len` fully random bytes (0..255, NUL bytes included
** in the middle on purpose: the lexer must be driven by an explicit
** length, never rely on strlen() internally for these).
*/
static void	fill_binary(char *buf, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		buf[i] = (char)(rand() % 256);
		i++;
	}
	buf[len] = '\0';
}

void	fuzz_phase_binary(t_mms *mms, const t_fuzz_cfg *cfg, t_fuzz_stats *st)
{
	char	buf[MAX_INPUT_SIZE + 1];
	size_t	i;
	size_t	len;

	printf("== [5] Binary / non-ASCII fuzz (%zu iters) ==\n",
		cfg->random_iters);
	i = 0;
	while (i < cfg->random_iters)
	{
		len = (size_t)rand() % MAX_INPUT_SIZE;
		fill_binary(buf, len);
		if (i % 50000 == 0)
			printf("[%zu/%zu]\n", i, cfg->random_iters);
		/*
		** NOTE: we pass ft_strlen(buf) as len for the printable-corpus
		** consistency of fuzz_run_one's stats, but if your lexer takes
		** an explicit length parameter instead of a NUL-terminated
		** string, replace this call so the *real* `len` (with embedded
		** NULs) is what gets fed to lexer(). As shipped, minishell's
		** lexer(char *line, ...) is NUL-terminated-string based, so
		** embedded NULs necessarily truncate the visible input - this
		** phase still stresses raw byte values >127 and control chars.
		*/
		fuzz_run_one(mms, buf, ft_strlen(buf), st);
		i++;
	}
	printf("\n");
}