/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:33:11 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/19 18:53:03 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include <stdio.h>
#include "ft_strings.h"

int     is_special_parameter(char c);
int     is_valid_ch(char c);
char    quoting_job(t_lexer *lexer)
{
    char    c;

    c = lexer->cmdl[lexer->index];
    if (c == '"')
    {
        lexer->tk->flags |= TOKF_DQUOTED;
        lexer->state |= ST_DQUOTED;
        return ('"');
    }
    else if (c == '\'')
    {
        lexer->tk->flags |= TOKF_SQUOTED;
        lexer->state |= ST_SQUOTED;
        return ('\'');
    }
    else if (c == '$')
    {
        lexer->index++;
        lexer->tk->flags |= TOKF_SQUOTED;
        lexer->state |= ST_SQUOTED;
        return ('\'');
    }
    return (0);
}

int is_quoting(t_lexer *lexer)
{
    char    c;

    c = lexer->cmdl[lexer->index];
    if (c == '\'' || '"' || '$')
        return (1);
    return (0);
}

/**
 * @brief Recognizes compound operators.
 * 
 * If the lexer state is currently in an operator state, this rule checks
 * whether the current character completes an operator
 * (&&, ||, << or >>). When successful, the character is appended
 * to the string builder, the appropriate token type is assigned,
 * and the lexer position is advanced.
 * 
 * @param lexer Lexer context struct
 * @param sb String builder
 * 
 * @return 1 if a compound operator is creted, otherwise it's 0
 */
int rule_op_continue(t_lexer *lexer, t_sb *sb)
{
    t_token *token;
    int     mask_flags;
    
    if (!lexer)
        return (0);
    token = stack_alloc(lexer->mms->sa, sizeof(t_token));
    mask_flags = ST_OP_AND | ST_OP_GREAT | ST_OP_LESS | ST_OP_OR;
    if (mask_flags & lexer->state && is_op(lexer->cmdl[lexer->index]) && token)
    {
        if (lexer->cmdl[lexer->index] == '&' && lexer->state & ST_OP_AND)
            token->type_tk =  TOK_AND_IF;
        else if (lexer->cmdl[lexer->index] == '|' && lexer->state & ST_OP_OR)
            token->type_tk =  TOK_OR_IF;
        else if (lexer->cmdl[lexer->index] == '<' && lexer->state & ST_OP_LESS)
            token->type_tk = TOK_DLESS;
        else if (lexer->cmdl[lexer->index] == '>' && lexer->state & ST_OP_GREAT)
            token->type_tk = TOK_DGREAT;
        append_ch_sb(sb, lexer->cmdl[lexer->index++]);
        token->value = sb->str;
        token->flags = 0;
        free(sb);
        sb = NULL;
        clear_op_state(&lexer->state);
        return (1);
    }
    return (0);
}

/**
 * @brief Check whether current character is an operator. 
 * 
 * If the lexer state is currently in an operator state and the the 
 * current character doesn't complete an operator. In this case,
 * the string builder buffer and the appropriates type are assigned
 * to token fields. The lexer position isn't advanced.
 * 
 * @param lexer Lexer context struct
 * @param sb String builder
 * 
 * @return 1 if a compound operator is creted, otherwise it's 0
 */
int     rule_op(t_lexer *lexer, t_sb *sb)
{
    t_token *token;
    int     mask_flags;
    
    if (!lexer)
        return (0);
    token = stack_alloc(lexer->mms->sa, sizeof(t_token));
    mask_flags = ST_OP_AND | ST_OP_GREAT | ST_OP_LESS | ST_OP_OR;
    if (mask_flags & lexer->state && !is_op(lexer->cmdl[lexer->index]) && token)
    {
        if (lexer->state & ST_OP_AND)
            token->type_tk =  TOK_AMPERSAND;
        else if (lexer->state & ST_OP_OR)
            token->type_tk =  TOK_OR_IF;
        else if (lexer->state & ST_OP_LESS)
            token->type_tk = TOK_LESS;
        else if (lexer->state & ST_OP_GREAT)
            token->type_tk = TOK_GREAT;
        token->value = sb->str;
        token->flags = 0;
        free(sb);
        sb = NULL;
        clear_op_state(&lexer->state);
        return (1);
    }
    return (0);
}

/**
 * @brief Token recognition under quoting mode.
 * 
 * If the current character is an unquoted backslash, an single quote,
 * a double quotes or a sequence of unquoted dollar single quote.
 * The next segment of text under theses quotes will be assigned to 
 * string builder buffer.
 * 
 * 
 * @param lexer Lexer context struct
 * @param sb String builder
 * 
 * @return 1 whether none error occured, otherwise it's 0.
 */
int rule_quoting(t_lexer *lexer, t_sb *sb)
{
    char    limit;
    
    if (!lexer)
        return (0);
    if (!is_quoting(lexer))
        return (0);
    limit = quoting_job(lexer);
    while (lexer->cmdl[lexer->index] != limit && lexer->cmdl[lexer->index])
    {
        append_ch_sb(sb, lexer->cmdl[lexer->index]);
        lexer->index++;
    }
    return (1);
}

/**
 * @brief Recognizes tokens while in expansion mode.
 *
 * When the current character is an unquoted '$' or '`', the following
 * text segment associated with the expansion is appended to the string
 * builder. A counter is maintained to track nested expansions. Although
 * nested expansion handling is not required for minishell, it was added
 * in anticipation of a future 42sh implementation.
 *
 * Note that several advanced expansion forms are not yet supported,
 * including command substitution and arithmetic expansion,
 * (`...`, $(...), $((...)), ${...})
 *
 * @param lexer Lexer context.
 * @param sb    String builder receiving the expanded content.
 *
 * @return 1 on success, 0 on error.
 */
int rule_expansion(t_lexer *lexer, t_sb *sb)
{
    t_token *token;
    int     mask_flags;
    int     count;
    
    if (!lexer)
        return (0);
    token = stack_alloc(lexer->mms->sa, sizeof(t_token));
    mask_flags = ST_BACK_TICK | ST_DQUOTED | ST_ESCAPED | ST_SQUOTED;
    count = 1;
    if ((lexer->state & mask_flags) || lexer->cmdl[lexer->index] != '$' || !token)
        return (0);
    append_ch_sb(sb, '$');
    while (count && lexer->cmdl[lexer->index])
    {
        lexer->index++;
        if (is_special_parameter(lexer->cmdl[lexer->index]) && lexer->cmdl[lexer->index - 1] == '$')
        {
            append_ch_sb(sb, lexer->cmdl[lexer->index]);
            token->type_tk = TOK_WORD;
            token->flags = TOKF_SPEC_PARAM;
            token->value = sb->str;
            free(sb);
            sb = NULL;
            count--;
        }
        else if (is_valid_ch(lexer->cmdl[lexer->index]))
            append_ch_sb(sb, lexer->cmdl[lexer->index]);
        else
            count--;
    }
    return (1);
}

/**
 * @brief Check whether current character begin an operator.
 * 
 * If the current character isn't quoted and can be a new operator,
 * The actual token is delimited, and an new token is started with 
 * the operator.
 * 
 * @param lexer Lexer context.
 * @param sb    String builder receiving the expanded content.
 *
 * @return 1 on success, 0 on error.
 */
int rule_beg_op(t_lexer *lexer, t_sb *sb)
{
    int     mask_flags;
    char    c;
    
    mask_flags = ST_BACK_TICK | ST_DQUOTED | ST_ESCAPED | ST_SQUOTED;
    if (!lexer || !sb || mask_flags)
        return (0);
    c = lexer->cmdl[lexer->index];
    if (!is_op(c))
        return (0);
    if (c == '&')
        lexer->state |= ST_OP_AND;
    else if (c == '|')
        lexer->state |= ST_OP_OR;
    else if (c == '<')
        lexer->state |= ST_OP_LESS;
    else if (c == '>')
        lexer->state |= ST_OP_GREAT;
    return (1);
}

/**
 * @brief Check whether current character begin an operator.
 * 
 * If the current character isn't quoted and is a blank,
 * The actual token is delimited, and an new token is started with 
 * the operator.
 * 
 * @param lexer Lexer context.
 * @param sb    String builder receiving the expanded content.
 *
 * @return 1 on success, 0 on error.
 */
int rule_blank_ch(t_lexer *lexer, t_sb *sb)
{
    int     mask_flags;
    
    mask_flags = ST_BACK_TICK | ST_DQUOTED | ST_ESCAPED | ST_SQUOTED;
    if (!lexer || !sb || mask_flags)
        return (0);
    if (lexer->cmdl[lexer->index] == ' ' || \
lexer->cmdl[lexer->index] == '\t' || lexer->cmdl[lexer->index] == '\n')
    {
        if (lexer->tk)
        {
            lexer->tk->value = sb->str;
            free(sb);
            lexer->tk->type_tk = TOK_WORD;
        }
        if (lexer->cmdl[lexer->index] == '\n')
        lexer->index++;
    }
    return (1);
}

/**
 * @brief Check whether current character compound a word.
 * 
 * @param lexer Lexer context.
 * @param sb    String builder receiving the expanded content.
 *
 * @return 1 on success, 0 on error.
 */
int rule_word_continue(t_lexer *lexer, t_sb *sb)
{
    if (!lexer || !sb)
        return (0);
    if (lexer->tk)
    {
        if (lexer->tk->flags & TOK_WORD)
        {
            append_ch_sb(sb, lexer->cmdl[lexer->index]);
            lexer->index++;
        }
    }
    return (1);
}

//int rule_commentary(t_mms *mms, int *state, char *cmdl, t_sb *sb);

int rule_begin_word(t_lexer *lexer, UNUSED t_sb *sb)
{
    lexer->tk = stack_alloc(lexer->mms->sa, sizeof(t_token));
    if (!lexer->tk)
        return (0);
    lexer->tk->flags |= TOK_WORD;
    return (1);
}

/**
 * @brief Loops through the command line to determine the rule associated
 * with each character.
 *
 * This function processes the command line read from the REPL (readline).
 * Using a greedy algorithm, it selects the first rule that matches the
 * current character. Characters are appended while the active rule does
 * not request parsing to stop.
 */
t_val_lexer    lexer(char *cmdl, t_mms *mms)
{
    t_sb            *sb;
    t_lexer         lexer;

    lexer.cmdl = cmdl;
    lexer.state = ST_NORMAL;
    lexer.index = 0;
    lexer.mms = mms;
    lexer.tk = NULL;
    lexer.tk->flags = 0;
    lexer.tk->type_tk = 0;
    lexer.tk->value = NULL;
    if (!mms || !cmdl)
        return (0);
    while (lexer.cmdl[lexer.index])
    {
        if (!sb)
            sb = init_sb(INIT_SIZE_SB);
        if (!sb)
            return (LX_ERROR);
        if (rule_op_continue(&lexer, sb) == 1)
            continue ;
        if (rule_op(&lexer, sb) == 1)
            continue ;
        if (rule_quoting(&lexer, sb) == 1)
            continue ;
        if (rule_expansion(&lexer, sb) == 1)
            continue ;
        if (rule_beg_op(&lexer, sb) == 1)
            continue ;
        if (rule_blank_ch(&lexer, sb))
            continue ;
        if (rule_word_continue(&lexer, sb))
            continue ;
        if (rule_begin_word(&lexer, sb))
            continue ;
    }
    return (1);
}