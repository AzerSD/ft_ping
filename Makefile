# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: userr <userr@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/20 04:17:53 by asioud            #+#    #+#              #
#    Updated: 2025/01/16 08:06:25 by userr            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ft_ping

CC			=	cc

RM			=	rm -rf

LIBFT		= 	libs/libft/libft.a

CFLAGS		=	-g -Wall -Wextra

SANITIZE	=	-fsanitize=address

HFILES		=	-I ./includes -I ./libs/libft/includes

SRC_DIR		=	srcs/

OBJ_DIR		=	obj/

SRC 		= $(notdir $(shell find $(SRC_DIR) -type f -name '*.c'))

SRCS 		= $(addprefix $(SRC_DIR), $(addsuffix .c, $(SRC)))
OBJS 		= $(addprefix $(OBJ_DIR), $(SRC:.c=.o))


all:	$(NAME)


$(NAME): $(LIBFT) $(OBJS)
	cd libs/libft && make
	$(CC) $(HFILES) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)


$(OBJ_DIR)%.o : $(SRC_DIR)%.c
	@mkdir -p $(@D)
	$(CC) $(HFILES) $(CFLAGS) -c $< -o $@


$(LIBFT):
	@git submodule update --init --recursive --remote
	@make -C libs/libft
	@echo "Libft installed"


clean:
	@$(RM) $(OBJ_DIR)
# @cd libs/libft && make clean


fclean:	clean
# @cd libs/libft && make fclean
	@$(RM) $(NAME)


re:	fclean all


.PHONY:	all bonus clean fclean re