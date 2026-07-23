# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/23 13:58:15 by fiaudfiz          #+#    #+#              #
#    Updated: 2026/07/23 13:58:15 by fiaudfiz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME     := exec_fuzz
TEST_DIR := /tmp/minishell_executor_fuzz

CC      := cc
CFLAGS  := -Wall -Wextra -g

ROOT    := ../../..
LIBFT   := $(ROOT)/lib/libft
INC     := -I$(ROOT)/include -I$(LIBFT)/include

SRCS    := main.c \
           fuzz.c \
           $(ROOT)/src/executor/executor.c \
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