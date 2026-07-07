# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/03 14:03:08 by miouali           #+#    #+#              #
#    Updated: 2026/07/07 13:31:19 by fiaudfiz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


MAKEFLAGS += --no-print-directory

# ------------------- DIRECTORIES ----------------- #
UNIT := unit
FUZZ := fuzz

# ---------------------- RULES -------------------- #

all: test

test: unit fuzz

unit:
	@echo "Running UNIT tests..."
	@make -C $(UNIT)

fuzz:
	@echo "Running FUZZ tests..."
	@make -C $(FUZZ)

# ------------------ CLEAN UP RULES --------------- #
clean:
	@make clean -C $(UNIT)
	@make clean -C $(FUZZ)

fclean:
	@make fclean -C $(UNIT)
	@make fclean -C $(FUZZ)

re: fclean all

.PHONY: all test unit fuzz clean fclean re