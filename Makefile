# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miouali <miouali@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/18 23:16:53 by gd-hallu          #+#    #+#              #
#    Updated: 2026/08/20 14:21:08 by miouali          ###   ########.fr        #
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
COMPILER_SH_LOWER := $(shell echo "$(COMPILER_SH)" | tr A-Z a-z)

ifeq ($(findstring clang, $(COMPILER_SH_LOWER)), clang)
	COMPILER		:= clang
else
	COMPILER		:= gcc
endif

# --------------------- FLAGS --------------------- #
ifeq ($(MODE),release)
	W_FLAGS			:= -Wall -Wextra -Werror -Wno-unused-result
else
	ifeq ($(COMPILER), clang)
		W_FLAGS			:= 	-Wall -Werror -Wextra -Wvla -Wmisleading-indentation -Wshadow -Wnull-dereference -fshort-enums
	else ifeq ($(COMPILER), gcc)
		W_FLAGS			:= 	-Wall -Werror -Wextra -Wvla -Wmisleading-indentation -Wstrict-aliasing=3 -Wduplicated-cond -Wstringop-overflow -Wshadow -Wnull-dereference -Warray-bounds -Wrestrict
	else
$(error Your device require clang or gcc to run the program)
	endif
endif

# --------------------- CFLAGS -------------------- #
ifeq ($(MODE),debug)
	CFLAGS := -g -O1 -std=c99 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
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
$(SRC)/$(CORE)/utils_0.c \
$(SRC)/$(CORE)/utils_1.c \
$(SRC)/$(CORE)/signaux.c \
$(SRC)/$(ENV)/ht_env.c \
$(SRC)/$(ENV)/converter.c \
$(SRC)/$(ENV)/set_env.c \
$(SRC)/$(ENV)/env_utils.c \
$(SRC)/$(LEXER)/lexer.c \
$(SRC)/$(LEXER)/lx_normal.c \
$(SRC)/$(LEXER)/lx_squote.c \
$(SRC)/$(LEXER)/lx_dquote.c \
$(SRC)/$(LEXER)/lx_operator.c \
$(SRC)/$(LEXER)/lx_utils_0.c \
$(SRC)/$(LEXER)/lx_utils_1.c \
$(SRC)/$(EXPANDER)/expander.c \
$(SRC)/$(EXPANDER)/expander_dquote.c \
$(SRC)/$(EXPANDER)/expander_squote.c \
$(SRC)/$(EXPANDER)/expander_expansion.c \
$(SRC)/$(EXPANDER)/expander_normal.c \
$(SRC)/$(EXPANDER)/expander_utils.c \
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
$(SRC)/$(EXECUTOR)/executor_heredoc_utils.c \
$(SRC)/$(EXECUTOR)/executor_path.c \
$(SRC)/$(EXECUTOR)/executor_pipeline_0.c \
$(SRC)/$(EXECUTOR)/executor_pipeline_1.c \
$(SRC)/$(EXECUTOR)/executor_redir.c \
$(SRC)/$(EXECUTOR)/executor_utils_0.c \
$(SRC)/$(EXECUTOR)/executor_utils_1.c \
$(SRC)/$(BUILTIN)/cd.c \
$(SRC)/$(BUILTIN)/echo.c \
$(SRC)/$(BUILTIN)/env.c \
$(SRC)/$(BUILTIN)/exit.c \
$(SRC)/$(BUILTIN)/export.c \
$(SRC)/$(BUILTIN)/export_utils.c \
$(SRC)/$(BUILTIN)/export_utils_2.c \
$(SRC)/$(BUILTIN)/pwd.c \
$(SRC)/$(BUILTIN)/unset.c \

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
	make -C $(LIBFT_D) CC_VERSION="GCC" W_FLAGS="-Wall -Werror -Wextra -Wno-error=unused-result"
	
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
