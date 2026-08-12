# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/30 23:54:37 by gd-hallu          #+#    #+#              #
#    Updated: 2026/07/23                                 ###   ########.fr      #
#                                                                              #
# **************************************************************************** #

# ------------------- COMPILER ------------------- #

CC      := cc
CFLAGS  := -Wall -Wextra -Werror -std=c99

RMF     := rm -f

# ------------------- DIRECTORIES ---------------- #

ROOT_PROJECT   := ../..
OBJ            := obj

LEXER          := lexer
EXPANDER       := expander
ENV            := env
PARSER         := parser
EXECUTOR       := executor_old

LIBFT          := $(ROOT_PROJECT)/lib/libft

# -------------------- HEADERS ------------------- #

CPPFLAGS := \
	-I$(ROOT_PROJECT)/include \
	-I$(ROOT_PROJECT)/tests/unit/executor \
	-I$(LIBFT)/include

# --------------------- LIBS --------------------- #

LDFLAGS := -L$(LIBFT)
LDLIBS  := -lft

# ---------------- OBJECT FILES ------------------ #

LEXER_OBJ := \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lexer.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_normal.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_dquote.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_squote.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_operator.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_utils_0.o \
$(ROOT_PROJECT)/$(OBJ)/$(LEXER)/lx_utils_1.o \

EXPANDER_OBJ := \
$(ROOT_PROJECT)/$(OBJ)/$(EXPANDER)/expander.o \
$(ROOT_PROJECT)/$(OBJ)/$(ENV)/ht_env.o \
$(ROOT_PROJECT)/$(OBJ)/$(ENV)/set_env.o \
$(ROOT_PROJECT)/$(OBJ)/$(ENV)/converter.o

PARSER_OBJ := \
$(ROOT_PROJECT)/$(OBJ)/$(PARSER)/parser_command.o \
$(ROOT_PROJECT)/$(OBJ)/$(PARSER)/parser_operator.o \
$(ROOT_PROJECT)/$(OBJ)/$(PARSER)/parser_utils.o \
$(ROOT_PROJECT)/$(OBJ)/$(PARSER)/parser.o

EXECUTOR_OBJ := \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_builtins.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_command_pipe.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_command.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_errors.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_heredoc.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_path.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_pipeline_0.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_pipeline_1.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_redir.o \
$(ROOT_PROJECT)/$(OBJ)/$(EXECUTOR)/executor_utils.o

# ---------------- UNIT SOURCES ------------------ #

LEXER_UNIT_SRC := \
$(LEXER)/unit_lexer.c

EXPANDER_UNIT_SRC := \
$(EXPANDER)/unit_expander.c

PARSER_UNIT_SRC := \
$(PARSER)/main.c

EXECUTOR_UNIT_SRC := \
$(EXECUTOR)/main.c \
$(EXECUTOR)/helpers.c

# ---------------- EXECUTABLES ------------------- #

LEXER_EXEC     := lexer_unit
EXPANDER_EXEC  := expander_unit
PARSER_EXEC    := parser_unit
EXECUTOR_EXEC  := executor_unit

# -------------------- RULES --------------------- #

all: lexer

root_project:
	$(MAKE) -C $(ROOT_PROJECT)

libft:
	$(MAKE) -C $(LIBFT)

lexer: root_project libft
	$(CC) $(CFLAGS) $(CPPFLAGS) \
	$(LEXER_UNIT_SRC) \
	$(LEXER_OBJ) \
	$(LDFLAGS) $(LDLIBS) \
	-o $(LEXER_EXEC)
	./$(LEXER_EXEC)

expander: root_project libft
	$(CC) $(CFLAGS) $(CPPFLAGS) \
	$(EXPANDER_UNIT_SRC) \
	$(EXPANDER_OBJ) \
	$(LDFLAGS) $(LDLIBS) \
	-o $(EXPANDER_EXEC)
	./$(EXPANDER_EXEC)

parser: root_project libft
	$(CC) $(CFLAGS) $(CPPFLAGS) \
	$(PARSER_UNIT_SRC) \
	$(PARSER_OBJ) \
	$(LDFLAGS) $(LDLIBS) \
	-o $(PARSER_EXEC)
	./$(PARSER_EXEC)

executor: root_project libft
	$(CC) $(CFLAGS) $(CPPFLAGS) \
	$(EXECUTOR_UNIT_SRC) \
	$(EXECUTOR_OBJ) \
	$(LDFLAGS) $(LDLIBS) \
	-o $(EXECUTOR_EXEC)
	./$(EXECUTOR_EXEC)

clean:
	$(RMF) \
	$(LEXER_EXEC) \
	$(EXPANDER_EXEC) \
	$(PARSER_EXEC) \
	$(EXECUTOR_EXEC)

re: clean all

.PHONY: all root_project libft lexer expander parser executor clean re