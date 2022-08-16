# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vmervin <vmervin@student-21.school.ru>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/23 17:20:35 by vmervin           #+#    #+#              #
#    Updated: 2022/08/16 16:10:41 by vmervin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 		= minishell
SRC_DIR		= ./srcs/
SRCS		= ${addprefix ${SRC_DIR}, built-ins.c environ.c	errors.c \
			utils2.c main.c command_memfree.c utils.c heredoc_proc.c prompt.c \
			var_process.c built-ins2.c init_pipe.c open_file.c executor.c \
			child_proc.c ${PARSER}}
PARSER_DIR	= parser/
PARSER		= ${addprefix ${PARSER_DIR}, command_parser.c \
			dollar_token.c equal_token.c parser.c pathname_expansion.c \
			quote_token.c space_redirect_token.c var_expansion.c word_token.c \
			expansion_empty.c analize_syntax.c}
HEADERS		= ${INCLUDES}/minishell.h
LIBFTPATH	= ./libft
LIBFT		= ${LIBFTPATH}/libft.a
INCLUDES 	= ./includes
OBJECTS		= ${addprefix ${OBJ_DIR}, ${patsubst %.c,%.o,${notdir ${SRCS}}}}
OBJ_DIR		= ./obj/
D_FILES		= ${patsubst %.o,%.d,${OBJECTS}}
CC 			= cc
DEBUG		= -g -pedantic
CFLAGS		= -Wall -Wextra  -Wshadow -Werror -I${INCLUDES} -MD
ifeq ($(shell uname -s), Linux)
CFLAGS2		= -L${LIBFTPATH} -lft -lncurses -lreadline # flags for linux
else
# CFLAGS2		= -L${LIBFTPATH} -lft -L${MACBOOK_READLINE}Cellar/readline/8.1.2/lib/ -I${MACBOOK_READLINE}Cellar/readline/8.1.2/include/ -lreadline
CFLAGS2		= -L${LIBFTPATH} -lft -L${SCHOOL_MAC_READLINE}Cellar/readline/8.1.2/lib/ -I${SCHOOL_MAC_READLINE}Cellar/readline/8.1.2/include/ -lreadline
MACBOOK_READLINE	= /opt/homebrew/
SCHOOL_MAC_READLINE = /Users/vmervin/.brew/
INC_FLAGS += -I$(HOME)/.brew/opt/readline/include
LIBS += -L$(HOME)/.brew/opt/readline/lib
endif
RM 			= rm -f

all:		$(NAME)

${NAME}:	${OBJECTS} ${LIBFT}
			${CC} ${CFLAGS} ${OBJECTS} ${CFLAGS2} -o $@

debug:		CFLAGS += ${DEBUG}
debug:		all

test:		CFLAGS += -DTEST
test:		all

${LIBFT}::
			${MAKE} -C ${LIBFTPATH}

${OBJ_DIR}%.o:	${SRC_DIR}%.c ${HEADERS} | ${OBJ_DIR}
			${CC} ${CFLAGS} -c $< -o $@

${OBJ_DIR}%.o:	${SRC_DIR}${PARSER_DIR}%.c ${HEADERS} | ${OBJ_DIR}
			${CC} ${CFLAGS} -c $<  -o $@

${OBJ_DIR}: 
			mkdir ${OBJ_DIR}

clean:	
			${RM} ${OBJECTS} ${D_FILES}
			${MAKE} clean -C ${LIBFTPATH}

fclean:		clean
			${RM} ${NAME}
			${RM} ${LIBFT}

re:			fclean all

.PHONY: clean fclean all re bonus debug
