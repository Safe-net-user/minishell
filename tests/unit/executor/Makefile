# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/22 17:29:04 by fiaudfiz          #+#    #+#              #
#    Updated: 2026/07/23 17:57:18 by fiaudfiz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



NAME     := exec_unit
TEST_DIR := /tmp/minishell_executor_tests

CC      := cc
CFLAGS  := -Wall -Wextra -g

ROOT    := ../../..
LIBFT   := $(ROOT)/lib/libft
INC     := -I$(ROOT)/include -I$(LIBFT)/include

SRCS    := main.c \
           helpers.c \
           $(ROOT)/src/executor/executor.c \
		   $(ROOT)/src/executor/executor_builtins.c \
		   $(ROOT)/src/executor/executor_command.c \
		   $(ROOT)/src/executor/executor_errors.c \
		   $(ROOT)/src/executor/executor_heredoc.c \
		   $(ROOT)/src/executor/executor_path.c \
		   $(ROOT)/src/executor/executor_pipeline_0.c \
		   $(ROOT)/src/executor/executor_pipeline_1.c \
		   $(ROOT)/src/executor/executor_redir.c \
		   $(ROOT)/src/executor/executor_utils.c \
		   $(ROOT)/src/executor/executor_command_pipe.c \
           $(ROOT)/src/env/ht_env.c \
           $(ROOT)/src/env/converter.c \
           $(ROOT)/src/env/set_env.c

OBJS    := $(SRCS:.c=.o)
LIB     := -L$(LIBFT) -lft -lreadline

all: $(TEST_DIR) $(LIBFT)/libft.a $(NAME)

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

$(LIBFT)/libft.a:
	$(MAKE) -C $(LIBFT)

$(NAME): $(OBJS) $(LIBFT)/libft.a
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFT) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT) fclean
	rm -rf $(TEST_DIR)

re: fclean all

.PHONY: all clean fclean re