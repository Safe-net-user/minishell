# ****************************************************************** #
#  Makefile.fuzz - build the lexer fuzzer against your minishell     #
#  usage:                                                            #
#    make -f Makefile.fuzz              # asan+ubsan build           #
#    make -f Makefile.fuzz plain        # no sanitizers              #
#    make -f Makefile.fuzz run          # build + run                #
#    make -f Makefile.fuzz SEED=42 run  # reproduce seed              #
# ****************************************************************** #

# -------------------- PROGRAM -------------------- #

NAME        := fuzz_lexer

CC          := cc

CFLAGS      := -Wall -Wextra -Werror -g3

SANFLAGS    := -fsanitize=address,undefined \
               -fno-omit-frame-pointer


# ------------------ PROJECT PATH ------------------ #

ROOT_PROJECT := ../../..

LIBFT        := $(ROOT_PROJECT)/lib/libft


# --------------------- HEADERS -------------------- #

INCLUDES := \
	-I. \
	-I$(ROOT_PROJECT)/include \
	-I$(LIBFT)/include


# -------------------- LIBRARIES ------------------ #

LDFLAGS := \
	-L$(LIBFT)

LDLIBS := \
	-lft


# -------------------- LEXER OBJ ------------------- #

LEXER_OBJS := \
	$(ROOT_PROJECT)/obj/lexer/lexer.o \
	$(ROOT_PROJECT)/obj/lexer/lx_normal.o \
	$(ROOT_PROJECT)/obj/lexer/lx_dquote.o \
	$(ROOT_PROJECT)/obj/lexer/lx_squote.o \
	$(ROOT_PROJECT)/obj/lexer/lx_operator.o


# -------------------- STACK OBJ ------------------ #
# Ajoute ici le fichier .o si ton allocator n'est pas
# inclus dans les objets du projet.

STACK_OBJS := \
	$(ROOT_PROJECT)/lib/libft/obj/stdlib/malloc/stack/*


# -------------------- FUZZ SRC ------------------- #

SRCS := \
	fuzz_lexer.c \
	fuzz_token.c \
	fuzz_mutate.c \
	fuzz_corpus.c

OBJS := $(SRCS:.c=.o)


# ---------------------- RULES -------------------- #

all: CFLAGS += $(SANFLAGS)
all: project $(NAME)


plain: project $(NAME)


project:
	$(MAKE) -C $(ROOT_PROJECT)


$(NAME): $(OBJS)
	$(CC) $(CFLAGS) \
		$(INCLUDES) \
		$(OBJS) \
		$(LEXER_OBJS) \
		$(STACK_OBJS) \
		$(LDFLAGS) \
		$(LDLIBS) \
		-o $(NAME)


%.o: %.c fuzz_lexer.h
	$(CC) $(CFLAGS) \
		$(INCLUDES) \
		-c $< \
		-o $@


run: all
	./$(NAME) $(SEED)


clean:
	rm -f $(OBJS)


fclean: clean
	rm -f $(NAME)


re: fclean all


.PHONY: all plain project run clean fclean re