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
ROOT_PROJECT	:= ../..
HDR_LEXER		:= $(ROOT_PROJECT)/include
LIBFT			:= $(ROOT_PROJECT)/lib/libft
SRC_LEXER_OBJ	:= \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lexer.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_normal.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_dquote.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_squote.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_operator.o \


# ---------------------- LIB --------------------- #
LIBFT_LIB			:= libft.a

# --------------------- FILES --------------------- #
LEXER_SRC		:= $(LEXER)/unit_lexer.c

# ------------------- EXEC NAME ------------------- #
LEXER_EXEC		:= lexer_unit

# -------------------- CFLAGS --------------------- #
CFLAGS			:= -Wall -Wextra -Werror -std=c99

# -------------------- HEADER --------------------- #
CPPFLAGS		:= -I $(HDR_LEXER) -I $(LIBFT)/include

# ------------------ CFLAGS LIB --------------------#
LDFLAGS			:= -L$(LIBFT)
LDLIBS			:= -lft

all: root_project $(LIBFT) lexer

root_project:
	make -C ../../

$(LIBFT):
	make -C $@

lexer: 
	@$(CC) $(CFLAGS) $(CPPFLAGS) $(LEXER_SRC) $(SRC_LEXER_OBJ) $(LDFLAGS) $(LDLIBS) -o $(LEXER_EXEC)
	./$(LEXER_EXEC)

clean:
	$(RMF) $(LEXER)/$(LEXER_EXEC)

re: clean all

.PHONY: all root_project lexer clean