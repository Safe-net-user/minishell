# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/21 01:11:37 by fiaudfiz          #+#    #+#              #
#    Updated: 2026/07/21 01:11:38 by fiaudfiz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    := exec_fuzz

CC      := cc
CFLAGS  := -Wall -Wextra -g

ROOT    := ../../..
LIBFT   := $(ROOT)/lib/libft
INC     := -I$(ROOT)/include -I$(LIBFT)/include

SRCS    := main.c \
           $(ROOT)/src/executor/executor.c \
           $(ROOT)/src/core/parser.c

OBJS    := $(SRCS:.c=.o)
LIB     := -L$(LIBFT) -lft -lreadline

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re