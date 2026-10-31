#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: olunichk <olunichk@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/27 20:57:56 by olunichk          #+#    #+#              #
#    Updated: 2026/10/28 20:47:26 by olunichk         ###   ########.fr        #
#                                                                              #
#******************************************************************************#

NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread -g3 -fsanitize=address -Iinclude
INCLUDES	= -Iinclude

SRC_DIR		= src
OBJ_DIR		= obj

SRCS		= main.c \
			  args.c \
			  args_check.c \
			  time_utils.c \
			  log.c \
			  heap.c \
			  heap_ops.c \
			  dongle.c \
			  dongle_wait.c \
			  coder.c \
			  coder_state.c \
			  coder_pair.c \
			  coder_dongles.c \
			  monitor.c \
			  sim.c \
			  sim_run.c \
			  sim_setup.c \
			  sim_free.c \

OBJS 		= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
HEADERS		= include/codexion.h include/heap.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
