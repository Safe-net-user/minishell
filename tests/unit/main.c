/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 14:33:39 by miouali           #+#    #+#             */
/*   Updated: 2026/07/20 13:29:24 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unit.h"

#define GREEN  "\033[32m"
#define RED    "\033[31m"
#define BLUE   "\033[34m"
#define CYAN   "\033[36m"
#define YELLOW "\033[33m"
#define RESET  "\033[0m"

#define ADD(type, val)                             \
do {                                               \
    tok = stack_alloc(mms->sa, sizeof(t_token));   \
    tok->type_tk = (type);                         \
    tok->value = (val);                            \
    tok->flags = 0;                                \
} while (0)

#define RUN(cmd)                                   \
do {                                               \
    printf("\n============================================================\n"); \
    printf("TEST : %s\n", cmd);                    \
    printf("============================================================\n"); \
    run_test(mms, cmd);                            \
    reset_sa(mms);                                 \
} while (0)

#define SECTION(name)                                                        \
    printf("\n\n" YELLOW "########## %s ##########" RESET "\n", name)

static const char *node_name(t_node_type type)
{
    if (type == NODE_CMD)
        return ("CMD");
    if (type == NODE_PIPE)
        return ("PIPE");
    if (type == NODE_AND)
        return ("AND");
    if (type == NODE_OR)
        return ("OR");
    return ("UNKNOWN");
}

static void print_indent(int depth)
{
    while (depth--)
        printf("│   ");
}

static void print_ast(t_ast *node, int depth)
{
    int     i;
    t_redir *r;

    if (!node)
        return ;

    print_indent(depth);
    printf(CYAN "├── " RESET);

    if (node->type == NODE_CMD)
    {
        printf(GREEN "CMD" RESET);

        i = 0;
        while (node->argv && node->argv[i])
            printf(" [%s]", node->argv[i++]);
        if (i == 0)
            printf(" (vide)");

        printf("\n");

        if (node->redirect)
        {
            r = node->redirect;
            while (r)
            {
                print_indent(depth + 1);
                if (r->type == TOK_DLESS)
                    printf(BLUE "<< %s" RESET "\n", r->file);
                else if (r->type == TOK_LESS)
                    printf(BLUE "< %s" RESET "\n", r->file);
                else if (r->type == TOK_DGREAT)
                    printf(BLUE ">> %s" RESET "\n", r->file);
                else
                    printf(BLUE "> %s" RESET "\n", r->file);
                r = r->next;
            }
        }
    }
    else
    {
        printf("%s\n", node_name(node->type));

        print_ast(node->left, depth + 1);
        print_ast(node->right, depth + 1);
    }
}

static void run_test(t_mms *mms, const char *title)
{
    t_ast *ast;

    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("🧪 %s\n", title);
    printf("═══════════════════════════════════════════════════════\n\n");

    ast = parser(mms);

    if (!ast)
    {
        printf(RED "✘ Parser returned NULL\n" RESET);
        return ;
    }

    printf(GREEN "✔ Parser OK\n\n" RESET);

    print_ast(ast, 0);

    printf("\n");
}

static t_stack_alloc *reset_sa(t_mms *mms)
{
    if (mms->sa)
        stack_dealloc(mms->sa);
    mms->sa = init_stack_allocator(32768);
    return (mms->sa);
}

int main(void)
{
    t_mms   *mms;
    t_token *tok;

    mms = calloc(1, sizeof(t_mms));
    mms->sa = init_stack_allocator(32768);

    /* ========================================================= */
    SECTION("Commandes simples");
    /* ========================================================= */

    ADD(TOK_WORD, "ls");
    ADD(TOK_EOF, NULL);
    RUN("ls");

    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "hello");
    ADD(TOK_WORD, "world");
    ADD(TOK_EOF, NULL);
    RUN("echo hello world");

    /* ========================================================= */
    SECTION("Pipes");
    /* ========================================================= */

    ADD(TOK_WORD, "ls");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "cat");
    ADD(TOK_EOF, NULL);
    RUN("ls | cat");

    ADD(TOK_WORD, "ls");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "grep");
    ADD(TOK_WORD, "foo");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "wc");
    ADD(TOK_EOF, NULL);
    RUN("ls | grep foo | wc");

    /* pipeline long, pour verifier l'associativite gauche sur N etages */
    ADD(TOK_WORD, "a");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "b");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "c");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "d");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "e");
    ADD(TOK_EOF, NULL);
    RUN("a | b | c | d | e  (assoc gauche sur 5 etages)");

    /* ========================================================= */
    SECTION("Operateurs logiques");
    /* ========================================================= */

    ADD(TOK_WORD, "ls");
    ADD(TOK_AND_IF, "&&");
    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "ok");
    ADD(TOK_EOF, NULL);
    RUN("ls && echo ok");

    ADD(TOK_WORD, "ls");
    ADD(TOK_OR_IF, "||");
    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "fail");
    ADD(TOK_EOF, NULL);
    RUN("ls || echo fail");

    /* melange && et || : verifier assoc gauche stricte, pas de precedence
       particuliere entre && et || dans cette grammaire */
    ADD(TOK_WORD, "a");
    ADD(TOK_AND_IF, "&&");
    ADD(TOK_WORD, "b");
    ADD(TOK_OR_IF, "||");
    ADD(TOK_WORD, "c");
    ADD(TOK_AND_IF, "&&");
    ADD(TOK_WORD, "d");
    ADD(TOK_EOF, NULL);
    RUN("a && b || c && d  (assoc gauche &&/||)");

    /* ========================================================= */
    SECTION("Pipes + logique combines");
    /* ========================================================= */

    ADD(TOK_WORD, "ls");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "grep");
    ADD(TOK_WORD, "foo");
    ADD(TOK_AND_IF, "&&");
    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "found");
    ADD(TOK_OR_IF, "||");
    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "nope");
    ADD(TOK_EOF, NULL);
    RUN("ls | grep foo && echo found || echo nope");

    /* le pipe doit lier plus fort que && / || des deux cotes */
    ADD(TOK_WORD, "a");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "b");
    ADD(TOK_AND_IF, "&&");
    ADD(TOK_WORD, "c");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "d");
    ADD(TOK_EOF, NULL);
    RUN("a | b && c | d  (pipe doit lier plus fort que &&)");

    /* ========================================================= */
    SECTION("Redirections simples (avant les mots -- cas qui marche)");
    /* ========================================================= */

    ADD(TOK_WORD, "cat");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "infile");
    ADD(TOK_EOF, NULL);
    RUN("cat < infile");

    ADD(TOK_WORD, "cat");
    ADD(TOK_DLESS, "<<");
    ADD(TOK_WORD, "EOF");
    ADD(TOK_EOF, NULL);
    RUN("cat << EOF");

    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "hello");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "outfile");
    ADD(TOK_EOF, NULL);
    RUN("echo hello > outfile");

    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "hello");
    ADD(TOK_DGREAT, ">>");
    ADD(TOK_WORD, "logfile");
    ADD(TOK_EOF, NULL);
    RUN("echo hello >> logfile");

    /* ========================================================= */
    SECTION("Redirections multiples / combinees");
    /* ========================================================= */

    ADD(TOK_WORD, "cat");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "in");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "out");
    ADD(TOK_EOF, NULL);
    RUN("cat < in > out");

    ADD(TOK_WORD, "cat");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "in1");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "in2");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "out1");
    ADD(TOK_DGREAT, ">>");
    ADD(TOK_WORD, "out2");
    ADD(TOK_EOF, NULL);
    RUN("cat < in1 < in2 > out1 >> out2  (plusieurs in ET plusieurs out)");

    /* deux redirections IDENTIQUES du meme type : en bash la derniere
       ecrase la premiere a l'execution, mais le PARSER doit quand meme
       les garder toutes les deux dans la liste chainee */
    ADD(TOK_WORD, "cat");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "out1");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "out2");
    ADD(TOK_EOF, NULL);
    RUN("cat > out1 > out2  (verifie que la liste chainee garde les 2)");

    /* ========================================================= */
    SECTION("redirection APRES ou AU MILIEU des mots");
    /* ========================================================= */

    /* parse_command ne consomme les redirs QUE dans la boucle
       AVANT les mots. Une fois qu'on rentre dans la boucle TOK_WORD,
       des qu'on croise un token de redirection on sort de la boucle
       et la fonction retourne direct -- rien ne revient consommer
       les tokens de redirection restants. Donc ici on s'attend a
       voir "REDIR_IN" / "REDIR_OUT" ABSENTS de l'affichage alors
       que la commande contient bien un '>' ou un '<'. */

    ADD(TOK_WORD, "cat");
    ADD(TOK_WORD, "file");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "out");
    ADD(TOK_EOF, NULL);
    RUN("cat file > out  (redir APRES un mot)");

    ADD(TOK_WORD, "echo");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "out");
    ADD(TOK_WORD, "extra");
    ADD(TOK_EOF, NULL);
    RUN("echo > out extra  (redir AU MILIEU des mots)");

    ADD(TOK_WORD, "cat");
    ADD(TOK_WORD, "a");
    ADD(TOK_WORD, "b");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "in");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "wc");
    ADD(TOK_EOF, NULL);
    RUN("cat a b < in | wc  (redir apres mots, avant un pipe)");

    /* ========================================================= */
    SECTION("Redirection seule, sans argv");
    /* ========================================================= */

    /* 2e commande du pipe n'a qu'un seul mot APRES sa redirection */
    ADD(TOK_WORD, "cat");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "infile");
    ADD(TOK_PIPE, "|");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "other");
    ADD(TOK_WORD, "wc");
    ADD(TOK_EOF, NULL);
    RUN("cat < infile | < other wc  (2e cmd n'a que 1 mot apres sa redir)");

    /* ========================================================= */
    SECTION("Pipe + redirection sur chaque etage");
    /* ========================================================= */

    ADD(TOK_WORD, "cat");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "input");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "grep");
    ADD(TOK_WORD, "foo");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "output");
    ADD(TOK_EOF, NULL);
    RUN("cat < input | grep foo > output  (2e cmd a sa redir APRES son mot)");

    ADD(TOK_WORD, "cat");
    ADD(TOK_DLESS, "<<");
    ADD(TOK_WORD, "EOF");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "grep");
    ADD(TOK_WORD, "foo");
    ADD(TOK_DGREAT, ">>");
    ADD(TOK_WORD, "log");
    ADD(TOK_EOF, NULL);
    RUN("cat << EOF | grep foo >> log");

    /* ========================================================= */
    SECTION("Chaines longues et complexes");
    /* ========================================================= */

    ADD(TOK_WORD, "ls");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "wc");
    ADD(TOK_AND_IF, "&&");
    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "done");
    ADD(TOK_OR_IF, "||");
    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "fail");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "cat");
    ADD(TOK_EOF, NULL);
    RUN("ls | wc && echo done || echo fail | cat");

    ADD(TOK_WORD, "cat");
    ADD(TOK_LESS, "<");
    ADD(TOK_WORD, "in");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "sort");
    ADD(TOK_PIPE, "|");
    ADD(TOK_WORD, "uniq");
    ADD(TOK_GREAT, ">");
    ADD(TOK_WORD, "out");
    ADD(TOK_AND_IF, "&&");
    ADD(TOK_WORD, "echo");
    ADD(TOK_WORD, "done");
    ADD(TOK_EOF, NULL);
    RUN("cat < in | sort | uniq > out && echo done  (pipeline 3 etages + redir finale)");

    stack_dealloc(mms->sa);
    free(mms);
    return (0);
}