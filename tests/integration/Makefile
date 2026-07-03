# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME        = integration_tests

CC          = cc
CFLAGS      = -Wall -Wextra -Werror

# **************************************************************************** #
#                                    PATHS                                     #
# **************************************************************************** #

ROOT        = ../..
LIBFT_DIR   = $(ROOT)/lib/libft
LIBFT       = $(LIBFT_DIR)/libft.a

INC = \
	-I. \
	-I$(ROOT)/include \
	-I$(LIBFT_DIR)/include

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRC = \
	main.c \
	$(ROOT)/src/parser/parser.c

# **************************************************************************** #

all: $(NAME)
	./$(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(SRC)
	$(CC) $(CFLAGS) $(INC) $(SRC) $(LIBFT) -o $(NAME)

clean:
	rm -f $(NAME)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re