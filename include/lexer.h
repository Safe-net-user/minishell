/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:49:01 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/18 15:24:26 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include "minishell.h"
#include "ft_string_builder.h"
#include <stdbool.h>

# define BUFFER_TOKEN 2048
# define INIT_SIZE_SB 64

typedef enum e_type_token {
    TOK_WORD,
    TOK_COMMAND,
    TOK_ASSIGNMENT_WORD,
    TOK_NAME,
    TOK_IO_NUMBER,
    TOK_IO_LOCATION,
    TOK_RESERVED_WORD,
    TOK_PIPE,
    TOK_AND_IF,         // &&
    TOK_OR_IF,          // ||
    TOK_AMPERSAND,      // &
    TOK_SEMI,           // ;
    TOK_DSEMI,          // ;;
    TOK_SEMI_AND,       // ;&
    TOK_LESS,           // <
    TOK_GREAT,          // >
    TOK_DLESS,          // <<
    TOK_DGREAT,         // >>
    TOK_LESSAND,        // <&
    TOK_GREATAND,       // >&
    TOK_LESSGREAT,      // <>
    TOK_DLESSDASH,      // <<-
    TOK_CLOBBER,        // >|
    TOK_LPAREN,         // (
    TOK_RPAREN,         // )
    TOK_NEWLINE,
    TOK_EOF
}	t_type_token;

typedef enum e_flag_token {
    TOKF_SQUOTED = 1 << 0,
    TOKF_DQUOTED = 1 << 1,
    TOKF_BACK_TICK = 1 << 2,
    TOKF_ARITH = 1 << 3,
    TOKF_COMMENT = 1 << 4,
    TOKF_ESCAPED = 1 << 5,
    TOKF_GLOB = 1 << 6,
    TOKF_TILDE = 1 << 7,
}   t_flag_token;

typedef enum e_state_lexer {
	ST_NORMAL = 1 << 0,
	ST_SQUOTED = 1 << 1,
	ST_DQUOTED = 1 << 2,
	ST_BACK_TICK = 1 << 3,
	ST_ARITH = 1 << 4,
	ST_COMMENT = 1 << 5,
	ST_OPERATOR_AND = 1 << 6,
    ST_OPERATOR_OR = 1 << 7,
	ST_ESCAPED = 1 << 8,
	ST_HERE_DOC = 1 << 9,
    ST_EXPENSION = 1 << 10,
} t_state_lexer;

typedef struct s_token {
	char		    *value;
	t_flag_token	flags;
	t_type_token	type_tk;
}   t_token;

int    lexer(char *cmdl, t_mms *mms);
bool    rule_op_continue(t_mms *mms, int *state, char *cmdl, t_sb *sb);

#endif