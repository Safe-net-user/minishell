/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:49:01 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/08/20 16:14:47 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include "type.h"
# include "ft_string_builder.h"
# include <stdbool.h>

# define BUFFER_TOKEN 2048

typedef enum e_type_tk
{
	TOK_WORD,
	TOK_COMMAND,
	TOK_ASSIGNMENT_WORD,
	TOK_NAME,
	TOK_IO_NUMBER,
	TOK_IO_LOCATION,
	TOK_RESERVED_WORD,
	TOK_DELIMITER,
	TOK_PIPE,
	TOK_AND_IF,
	TOK_OR_IF,
	TOK_AMPERSAND,
	TOK_SEMI,
	TOK_DSEMI,
	TOK_SEMI_AND,
	TOK_LESS,
	TOK_GREAT,
	TOK_DLESS,
	TOK_DGREAT,
	TOK_LESSAND,
	TOK_GREATAND,
	TOK_LESSGREAT,
	TOK_DLESSDASH,
	TOK_CLOBBER,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_NEWLINE,
	TOK_EOF
}	t_type_tk;

typedef enum e_flag_tk
{
	TOKF_SQUOTE = 1 << 0,
	TOKF_DQUOTE = 1 << 1,
	TOKF_BACK_TICK = 1 << 2,
	TOKF_ARITH = 1 << 3,
	TOKF_COMMENT = 1 << 4,
	TOKF_ESCAPE = 1 << 5,
	TOKF_SPEC_PARAM = 1 << 6,
	TOKF_EXPANSION = 1 << 7,
	TOKF_TILDE = 1 << 8,
}	t_flag_tk;

typedef enum e_state_lx
{
	LX_NORMAL,
	LX_SQUOTE,
	LX_DQUOTE,
	LX_OPERATOR,
}	t_state_lx;

typedef enum e_val_lx
{
	LX_SUCCESS,
	LX_ERROR,
	LX_DQUOTE_NF,
	LX_SQUOTE_NF,
	LX_BQUOTE_NF,
}	t_val_lx;

/**
 * @brief Represents a token in the shell lexer.
 *
 * Stores the token value, its type and flags, as well as links
 * to the previous and next tokens. It also stores heredoc content
 * when the token is associated with a heredoc.
 *
 * @param value Token string value.
 * @param flags Token flags.
 * @param type_tk Token type.
 * @param next Pointer to the next token.
 * @param prev Pointer to the previous token.
 * @param heredoc_content Heredoc content associated with the token.
 */

typedef struct s_tk
{
	char			*value;
	struct s_tk		*next;
	struct s_tk		*prev;
	char			*heredoc_content;
	t_flag_tk		flags;
	t_type_tk		type_tk;
}	t_tk;

/**
 * @struct s_lx
 * @brief Stores the lexer state and its working resources.
 *
 * Contains the minishell context, command line being parsed, current
 * token, string builder and current position in the input. It also
 * stores the current lexer state and whether the next token must be
 * interpreted as a heredoc delimiter.
 */

typedef struct s_lx
{
	t_mms	*mms;
	char	*cmdl;
	t_tk	*tk;
	t_sb	*sb;
	size_t	index;
	int		state;
	bool	is_next_delimiter;
}	t_lx;

typedef t_val_lx	(*t_state_lx_fn)(t_lx *);

t_val_lx			lexer(char *cmdl, t_mms *mms);
int					emit_tk(t_lx *lx);
t_lx				*init_s_lx(char *cmdl, t_mms *mms);

t_val_lx			lx_normal(t_lx *lx);
t_val_lx			lx_squote(t_lx *lx);
t_val_lx			lx_dquote(t_lx *lx);
t_val_lx			lx_operator(t_lx *lx);

int					emit_tk(t_lx *lx);
int					emit_eof(t_lx *lx);
t_lx				*init_s_lx( char *cmdl, t_mms *mms);
void				free_lexer(t_lx *lx);
bool				is_no_value_tk(t_type_tk type);

#endif
