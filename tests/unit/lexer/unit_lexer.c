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
#include "minishell.h"
#include "ft_string_builder.h"
#include "gnl.h"

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
	mms->sa = init_stack_allocator(INIT_SIZE_SA);
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

int	check_token(t_mms *mms, char *str)
{
	t_sb	*sb;
	t_token	*tok;
	
	sb = init_sb(64);
	if (!sb)
		return (0);
	tok = mms->sa->buffer;
	while (tok < mms->sa->up)
	{
		append_sb(sb, tok->value);
		append_ch_sb(sb, ',');
		append_sb(sb, ft_atoi(tok->type_tk));
		append_ch_sb(sb, ',');
		append_sb(sb, ft_atoi(tok->flags));
		append_ch_sb(sb, ',');
		tok += sizeof(t_token);
	}
	
}

int	run_file(t_mms *mms, char *infile, char *outfile)
{
	int		fd_in;
	int		fd_out;
	char	*str_in;
	char	*str_out;
	
	fd_in = open(infile, O_RDONLY);
	fd_out = open(outfile, O_RDONLY);
	str_in = get_next_line(fd_in);
	while (str_in)
	{
		lexer(str_in, mms);
		if (!check_token(mms, get_next_line(str_out)))
			return (0);
		free(str_in);
		free(str_out);
		get_next_line(str_in);
	}
	return (1);
}

int	run_basic_tests(t_mms *mms)
{
	printf("|------------- basic test -----------|\n");
	if (run_file(mms, "basic_in.txt", "basic_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_space_tests(t_mms *mms)
{
	printf("|------------- space test -----------|\n");
	if (run_file(mms, "space_in.txt", "space_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_squote_tests(t_mms *mms)
{
	printf("|------------- squote test -----------|\n");
	if (run_file(mms, "squote_in.txt", "squote_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_dquote_tests(t_mms *mms)
{
	printf("|------------- dquote test -----------|\n");
	if (run_file(mms, "dquote_in.txt", "dquote_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_expansion_tests(t_mms *mms)
{
	printf("|----------- expansion test ----------|\n");
	if (run_file(mms, "expansion_in.txt", "expansion_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_redirection_tests(t_mms *mms)
{
	printf("|---------- redirection test ----------|\n");
	if (run_file(mms, "redirection_in.txt", "redirection_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_word_tests(t_mms *mms)
{
	printf("|------------- words test ------------|\n");
	if (run_file(mms, "words_in.txt", "words_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_pipe_tests(t_mms *mms)
{
	printf("|----------- pipe test ----------|\n");
	if (run_file(mms, "word_in.txt", "word_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
	return (0);
}

int	run_allquotes_tests(t_mms *mms)
{
	printf("|--------- allquotes test --------|\n");
	if (run_file(mms, "allquotes_in.txt", "allquotes_out.txt"))
	{
		printf("Good");
		return (1);
	}
	printf("Error");
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
	
    free_og_struct(mms);
    return (1);
}