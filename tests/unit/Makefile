# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/30 23:54:37 by gd-hallu          #+#    #+#              #
#    Updated: 2026/07/01 01:25:05 by gd-hallu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ------------------- CONSTANTS ------------------- #
CC				:= cc
RMF				:= rm -f
RMRF			:= rm -rf

# ------------------- DIRECTORIES ----------------- #
OBJ				:= obj
LEXER			:= lexer
EXPANDER		:= expander
ENV				:= env
ROOT_PROJECT	:= ../..
HDRS			:= $(ROOT_PROJECT)/include
LIBFT			:= $(ROOT_PROJECT)/lib/libft

LEXER_OBJ	:= \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lexer.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_normal.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_dquote.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_squote.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_operator.o \

EXPANDER_OBJ	:= $(ROOT_PROJECT)/$(OBJ)/$(EXPANDER)/expander.o \
$(ROOT_PROJECT)/$(OBJ)/$(ENV)/ht_env.o \
$(ROOT_PROJECT)/$(OBJ)/$(ENV)/set_env.o \
$(ROOT_PROJECT)/$(OBJ)/$(ENV)/converter.o \



# ---------------------- LIB --------------------- #
LIBFT_LIB			:= libft.a

# --------------------- FILES --------------------- #
LEXER_UNIT_SRC		:= $(LEXER)/unit_lexer.c
EXPANDER_UNIT_SRC	:= $(EXPANDER)/unit_expander.c

# ------------------- EXEC NAME ------------------- #
LEXER_EXEC		:= lexer_unit
EXPANDER_EXEC	:= expander_unit

# -------------------- CFLAGS --------------------- #
CFLAGS			:= -Wall -Wextra -Werror -std=c99

# -------------------- HEADER --------------------- #
CPPFLAGS		:= -I $(HDRS) -I $(LIBFT)/include

# ------------------ CFLAGS LIB --------------------#
LDFLAGS			:= -L$(LIBFT)
LDLIBS			:= -lft

all: root_project $(LIBFT) lexer

root_project:
	make -C ../../

$(LIBFT):
	make -C $@

lexer: root_project
	@$(CC) $(CFLAGS) $(CPPFLAGS) $(LEXER_UNIT_SRC) $(SRC_LEXER_OBJ) $(LDFLAGS) $(LDLIBS) -o $(LEXER_EXEC)
	./$(LEXER_EXEC)

expander: root_project
	@$(CC) $(CFLAGS) $(CPPFLAGS) $(EXPANDER_UNIT_SRC) $(EXPANDER_OBJ) $(LDFLAGS) $(LDLIBS) -o $(EXPANDER_EXEC)
	./$(EXPANDER_EXEC)
clean:
	$(RMF) $(LEXER)/$(LEXER_EXEC)

re: clean all

.PHONY: all root_project lexer expander clean