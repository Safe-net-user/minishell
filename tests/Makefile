# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miouali <miouali@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/03 14:03:08 by miouali           #+#    #+#              #
#    Updated: 2026/07/03 14:03:10 by miouali          ###   ########.fr        #
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