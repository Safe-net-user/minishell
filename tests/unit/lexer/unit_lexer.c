/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unit_lexer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 20:26:09 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/07/01 04:52:13 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "ft_strings.h"
#include "minishell.h"
#include "ft_string_builder.h"
#include "gnl.h"
#include "ft_stdlib.h"
#include <unistd.h>
#include <stdint.h>

#define UNUSED __attribute__((unused))

void    free_og_struct(t_mms *mms)
{
    if (!mms)
        return ;
}

static	t_mms	*init_og_struct(void)
{
	t_mms	*mms;

	mms = malloc(sizeof(t_mms));
	if (!mms)
		return (0);
	mms->env = NULL;
	mms->alias = NULL;
	mms->cmd_path = NULL;
	mms->sa = init_stack_allocator(32768);
	//printf("INIT Adresse mms->sa->buffer -> %p\n", (void *)mms->sa->buffer);
	mms->cwd = NULL;
	mms->name = NULL;
	mms->last_status = 0;
	mms->umask = 0022;
	if (!mms->sa)
	{
		free_og_struct(mms);
		return (NULL);
	}
	return (mms);
}

int	check_token(t_mms *mms, char *expected)
{
	t_sb	*sb;
	t_tk	*tok;
	int		ok;

	sb = init_sb(DEFAULT_SB_SIZE);
	if (!sb)
		return (0);
	tok = (t_tk *)((uint8_t *)mms->sa->buffer + sizeof(t_header));
	while (tok < (t_tk *)mms->sa->up)
	{
		append_sb(sb, tok->value);
		append_ch_sb(sb, ',');
		append_sb(sb, ft_itoa(tok->type_tk));
		append_ch_sb(sb, ',');
		append_sb(sb, ft_itoa(tok->flags));
		append_ch_sb(sb, ',');
		tok = (t_tk *)((uint8_t *)tok + sizeof(t_header) + sizeof(t_tk));
	}
	ok = (ft_strcmp(sb->str, expected) == 0);
	if (ok)
		printf("[OK] STR -> %s | EXPEXCTED -> %s\n", sb->str, expected);
	else
	{
		printf("[KO]\n");
		printf("sb str ->%s\n", sb->str);
		printf("expected ->%s\n", expected);
	}
	free(sb->str);
	free(sb);
	while (mms->sa->curr)
		stack_dealloc(mms->sa);
	return (ok);
}

int	run_file(t_mms *mms, char *infile, char *outfile)
{
	int		fd_in;
	int		fd_out;
	char	*input;
	char	*expected;

	fd_in = open(infile, O_RDONLY);
	fd_out = open(outfile, O_RDONLY);
	if (fd_in < 0 || fd_out < 0)
		return (0);
	input = get_next_line(fd_in);
	input[ft_strlen(input)] = '\0';
	expected = get_next_line(fd_out);
	while (input && expected)
	{
		if (input[ft_strlen(input) - 1] == '\n')
			input[ft_strlen(input) - 1] = '\0';
		if (expected[ft_strlen(expected) - 1] == '\n')
			expected[ft_strlen(expected) - 1] = '\0';

		lexer(input, mms);
		if (!check_token(mms, expected))
		{
			free(input);
			free(expected);
			close(fd_in);
			close(fd_out);
			return (0);
		}
		free(input);
		free(expected);
		input = get_next_line(fd_in);
		expected = get_next_line(fd_out);
	}
	close(fd_in);
	close(fd_out);
	return (1);
}
int	run_basic_tests(t_mms *mms)
{
	printf("|------------ basic test -----------|\n");
	if (run_file(mms, "./lexer/in/basic_in.txt", "./lexer/out/basic_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_space_tests(t_mms *mms)
{
	printf("|------------- space test ----------|\n");
	if (run_file(mms, "./lexer/in/space_in.txt", "./lexer/out/space_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_squote_tests(t_mms *mms)
{
	printf("|------------ squote test ----------|\n");
	if (run_file(mms, "./lexer/in/squote_in.txt", "./lexer/out/squote_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_dquote_tests(t_mms *mms)
{
	printf("|------------ dquote test ----------|\n");
	if (run_file(mms, "./lexer/in/dquote_in.txt", "./lexer/out/dquote_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_expansion_tests(t_mms *mms)
{
	printf("|---------- expansion test ---------|\n");
	if (run_file(mms, "./lexer/in/expansion_in.txt", "./lexer/out/expansion_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_redirection_tests(t_mms *mms)
{
	printf("|-------- redirection test ---------|\n");
	if (run_file(mms, "./lexer/in/redirection_in.txt", "./lexer/out/redirection_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_word_tests(t_mms *mms)
{
	printf("|------------ words test -----------|\n");
	if (run_file(mms, "./lexer/in/word_in.txt", "./lexer/out/word_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_pipe_tests(t_mms *mms)
{
	printf("|------------- pipe test -----------|\n");
	if (run_file(mms, "./lexer/in/pipe_in.txt", "./lexer/out/pipe_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int	run_allquotes_tests(t_mms *mms)
{
	printf("|---------- allquotes test ---------|\n");
	if (run_file(mms, "./lexer/in/allquotes_in.txt", "./lexer/out/allquotes_out.txt"))
	{
		printf("Good\n");
		return (1);
	}
	printf("Error\n");
	return (0);
}

int main(void)
{
    t_mms *mms;

    mms = init_og_struct();
    if (!mms)
        return (EXIT_FAILURE);

    printf("#####################################\n");
    printf("#        LEXER TESTS UNITAIRES      #\n");
    printf("#####################################\n");

    run_basic_tests(mms);
	run_space_tests(mms);
    run_squote_tests(mms);
    run_dquote_tests(mms);
    run_expansion_tests(mms);
    run_redirection_tests(mms);
	run_word_tests(mms);
	run_pipe_tests(mms);
	run_allquotes_tests(mms);

    //free_og_struct(mms);
    return (0);
}