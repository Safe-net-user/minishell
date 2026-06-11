/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:49:01 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/11 16:56:55 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

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
    SQUOTED = 1 << 0,
    DQUOTED = 1 << 1,
    BACK_TICK = 1 << 2,
    ARITH = 1 << 3,
    COMMENT = 1 << 4,
    ESCAPED = 1 << 5,
    GLOB = 1 << 6,
    TILDE = 1 << 7
}   t_flag_token;

typedef enum e_state_lexer {
	NORMAL = 1 << 0,
	SQUOTED = 1 << 1,
	DQUOTED = 1 << 2,
	BACK_TICK = 1 << 3,
	ARITH = 1 << 4,
	COMMENT = 1 << 5,
	OPERATOR = 1 << 6,
	ESCAPED = 1 << 7,
	HERE_DOC = 1 << 8,
    EXPENSION = 1 << 9,
} t_state_lexer;

typedef struct s_token {
	char		    *value;
	t_flag_token	flags;
	t_type_token	type_tk;
}   t_token;
#endif