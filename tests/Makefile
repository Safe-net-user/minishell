# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/06/30 18:45:53 by gd-hallu          #+#    #+#              #
#    Updated: 2026/06/30 18:51:11 by gd-hallu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAKEFLAGS += --no-print-directory

# ------------------- DIRECTORIES ----------------- #
UNIT := unit
INT  := integration
FUZZ := fuzz

# ---------------------- RULES -------------------- #

all: test

test: unit fuzz integration

unit:
	@echo "Running UNIT tests..."
	@make -C $(UNIT)

integration:
	@echo "Running INTEGRATION tests..."
	@make-C $(INT)

fuzz:
	@echo "Running FUZZ tests..."
	@make -C $(FUZZ)

# ------------------ CLEAN UP RULES --------------- #
clean:
	@make clean -C $(UNIT)
	@make clean -C $(INT)
	@make clean -C $(FUZZ)

fclean:
	@make fclean -C $(UNIT)
	@make fclean -C $(INT)
	@make fclean -C $(FUZZ)

re: fclean all

.PHONY: all test unit integration fuzz clean fclean re