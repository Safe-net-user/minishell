# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/18 23:16:53 by gd-hallu          #+#    #+#              #
#    Updated: 2026/07/23 18:17:26 by fiaudfiz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAKEFLAGS		+= --no-print-directory

# ------------------ PROJECT NAME ----------------- #
NAME			:= minishell

# ------------------- CONSTANTS ------------------- #
CC				:= cc
MKDIRP			:= mkdir -p
RMRF			:= rm -rf
RMF				:= rm -f

# ------------------- DIRECTORIES ----------------- #
HDR				:= include
LIB				:= lib
LIBFT_D			:= $(LIB)/libft
OBJ				:= obj
DEP				:= dep
SRC				:= src
ASM				:= asm

# ----------------- SRC DIRECTORIES --------------- #
CORE			:= core
BUILTIN			:= builtin
LEXER			:= lexer
PARSER			:= parser
REPL			:= repl
UTILS			:= utils
ENV				:= env
EXPANDER		:= expander
EXECUTOR		:= executor

# ---------------------- LIB --------------------- #
LIBFT			:= libft.a

# ---------------------- MODE -------------------- #
MODE			?= release

# -------------------- SELL CMD ------------------- #
COMPILER_SH		:= $(shell $(CC) --version)

# ------------------- COMPILER -------------------- #
ifeq ($(findstring clang, $(COMPILER_SH)), clang)
	COMPILER		:= clang
else ifeq ($(findstring GCC, $(COMPILER_SH)), GCC)
	COMPILER		:= gcc
else
	@echo "Your device require clang or gcc to run the program"
	@exit 1
endif

# --------------------- FLAGS --------------------- #
ifeq ($(MODE),release)
	W_FLAGS			:= -Wall -Wextra
else
	ifeq ($(COMPILER), clang)
		W_FLAGS			:= 	-Wall -Werror -Wextra -Wvla -Wpedantic -pedantic-errors -Wmisleading-indentation -Wsign-conversion -Wshadow -Wnull-dereference -fshort-enums
	else ifeq ($(COMPILER), gcc)
		W_FLAGS			:= 	-Wall -Werror -Wextra -Wvla -Wpedantic -pedantic-errors -Wmisleading-indentation -Wsign-conversion -Wstrict-aliasing=3 -Wduplicated-cond -Wstringop-overflow -Wshadow -Wnull-dereference -Warray-bounds -Wrestrict -Wconversion
	else
		@echo "Your device require clang or gcc to run the program"
		@exit 1
	endif
endif

# --------------------- CFLAGS -------------------- #
ifeq ($(MODE),debug)
	CFLAGS := -g -O1 -std=c99
else ifeq ($(MODE),debug_memory)
	CFLAGS := -g -O1 -std=c99 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
else ifeq ($(MODE),debug_thread)
	CFLAGS := -g -O1 -std=c99 -fsanitize=thread -fno-omit-frame-pointer
else ifeq ($(MODE),release)
	CFLAGS := -O3 -std=c99
endif

# --------------------- FILES --------------------- #
SOURCES 			:= \
$(SRC)/$(CORE)/main.c \
$(SRC)/$(CORE)/free.c \
$(SRC)/$(ENV)/ht_env.c \
$(SRC)/$(ENV)/converter.c \
$(SRC)/$(ENV)/set_env.c \
$(SRC)/$(CORE)/signaux.c \
$(SRC)/$(LEXER)/lexer.c \
$(SRC)/$(LEXER)/lx_normal.c \
$(SRC)/$(LEXER)/lx_squote.c \
$(SRC)/$(LEXER)/lx_dquote.c \
$(SRC)/$(LEXER)/lx_operator.c \
$(SRC)/$(EXPANDER)/expander.c \
$(SRC)/$(PARSER)/parser.c \
$(SRC)/$(PARSER)/parser_command.c \
$(SRC)/$(PARSER)/parser_operator.c \
$(SRC)/$(PARSER)/parser_utils.c \
$(SRC)/$(EXECUTOR)/executor.c \
$(SRC)/$(EXECUTOR)/executor_builtins.c \
$(SRC)/$(EXECUTOR)/executor_command_pipe.c \
$(SRC)/$(EXECUTOR)/executor_command.c \
$(SRC)/$(EXECUTOR)/executor_errors.c \
$(SRC)/$(EXECUTOR)/executor_heredoc.c \
$(SRC)/$(EXECUTOR)/executor_path.c \
$(SRC)/$(EXECUTOR)/executor_pipeline_0.c \
$(SRC)/$(EXECUTOR)/executor_pipeline_1.c \
$(SRC)/$(EXECUTOR)/executor_redir.c \
$(SRC)/$(EXECUTOR)/executor_utils.c

# -------------------- OBJECTS -------------------- #
OBJECTS 			:= $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# ------------------- DEPENDENCE ------------------ #
DEPENDENCE			:= $(patsubst $(SRC)/%.c, $(DEP)/%.d, $(SOURCES))

# ------------------- ASM FILES ------------------- #
ASM_FILES			:= $(patsubst $(SRC)/%.c, $(ASM)/%.s, $(SOURCES))

# ---------------- COMPILATION RULES -------------- #
all: $(NAME)

-include $(DEPENDENCE)

$(LIBFT):
	make -C $(LIBFT_D)
	
$(NAME): $(LIBFT) $(OBJECTS)
	@$(CC) $(W_FLAGS) $(CFLAGS) $(OBJECTS) -L$(LIBFT_D) -lft -lreadline -I$(HDR) -I$(LIBFT_D)/include -o $(NAME)

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(W_FLAGS) $(CFLAGS) -MMD -MP -I$(HDR) -I$(LIBFT_D)/include -c $< -o $@

# -------------- COMPILATION ASM RULES ------------ #
f_asm: $(ASM_FILES)

$(ASM)/%.s: $(SRC)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(W_FLAGS) $(CFLAGS) -I$(HDR) -S $< -o $@
	
# ------------------ CLEAN UP RULES --------------- #
clean:
	$(RMRF) $(OBJ)
	$(RMRF) $(ASM)
	make clean -C $(LIBFT_D)
	
fclean: clean
	$(RMF) $(NAME)
	make fclean -C $(LIBFT_D)

re: fclean all
	make re -C $(LIBFT_D)

.PHONY: all f_asm re fclean clean