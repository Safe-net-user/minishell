/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:12:16 by miouali           #+#    #+#             */
/*   Updated: 2026/07/06 16:54:36 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fuzz.h"

static void generate_case(t_mms *mms)
{
	t_token	*tok;
	int		n;
	int		i;

	n = rand() % 15 + 1;
	i = 0;
	while (i < n)
	{
		tok = stack_alloc(mms->sa, sizeof(t_token));
		if (!tok)
			return ;

		if (rand() % 3 == 0)
		{
			tok->type_tk = ops[rand() % OPS_COUNT];
			tok->value = NULL;
		}
		else
		{
			tok->type_tk = TOK_WORD;
			tok->value = (char *)words[rand() % WORDS_COUNT];
		}
		tok->flags = 0;
		i++;
	}

	tok = stack_alloc(mms->sa, sizeof(t_token));
	if (!tok)
		return ;
	tok->type_tk = TOK_EOF;
	tok->value = NULL;
	tok->flags = 0;
}

static void print_tokens(t_mms *mms)
{
    t_token *tok;

    tok = (t_token *)(mms->sa->buffer + sizeof(t_header));

    while (1)
    {
        switch (tok->type_tk)
        {
            case TOK_WORD:
                printf(GREEN "%s" RESET, tok->value);
                break;
            case TOK_PIPE:
                printf(CYAN "|");
                break;
            case TOK_AND_IF:
                printf(CYAN " &&");
                break;
            case TOK_OR_IF:
                printf(CYAN " ||");
                break;
            case TOK_LESS:
                printf(CYAN " <");
                break;
            case TOK_GREAT:
                printf(CYAN " >");
                break;
            case TOK_DLESS:
                printf(CYAN " <<");
                break;
            case TOK_DGREAT:
                printf(CYAN " >>");
                break;
            case TOK_EOF:
                printf(YELLOW " <EOF>" RESET);
                printf("\n");
                return ;
            default:
                printf(RED " ?");
        }

        printf(" ");
        tok = next_token(tok);
    }
}

int main(void)
{
    t_mms  *mms;
    int     i;
    int     ok;

    srand(time(NULL));

    mms = calloc(1, sizeof(t_mms));
    ok = 0;

    for (i = 0; i < 100; i++)
    {
        mms->sa = init_stack_allocator(32768);

        generate_case(mms);

        printf("\n--------------------------------------------\n");
        printf("FUZZ %03d : ", i + 1);
        print_tokens(mms);

        if (parser(mms))
        {
            printf(GREEN "PASS\n" RESET);
            ok++;
        }
        else
            printf(YELLOW "Syntax error (expected)\n" RESET);

        stack_dealloc(mms->sa);
    }

    free(mms);

    printf("\n============================================\n");
    printf(GREEN "Successful parses : %d\n" RESET, ok);
    printf(YELLOW "Syntax errors     : %d\n" RESET, 100 - ok);
    printf("============================================\n");

    return (0);
}