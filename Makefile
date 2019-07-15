# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/03/03 12:06:08 by jbeall            #+#    #+#              #
#    Updated: 2019/06/28 11:59:45 by jbeall           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#==================================== GENERAL =================================#

NAME = new-bin
CC = clang
CPPFLAGS = -Wall -Wextra -Werror #-g
LDFLAGS = -pipe -flto=full #-fsanitize=address,undefined
OPT = -O3 -flto=full -march=native #-fsanitize=address,undefined
SUB = libft

#=================================== SOURCES ==================================#

LIST = main
VPATH = src
SRC = $(addsuffix .c, $(LIST))

#=================================== OBJECTS ==================================#

OBJ_DIR = .obj/
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.c=.o))
DEP = $(OBJ:%.o=%.d)

#================================== LIBRARIES =================================#

LIBFT = -L ./libft/ -lft

#=================================== HEADERS ==================================#

INC_DIR = ./includes/
LIB_INC = libft/includes/
INC = -I $(LIB_INC) -I $(INC_DIR)

#==================================== COLORS ==================================#

#COLORS
COM_COLOR   = \033[92m
NO_COLOR    = \033[m

#TEXT
COM_STRING  = "$(NAME) compilation successful"
CLEAN_OBJ	= "cleaned $(NAME) objects"
CLEAN_NAME	= "cleaned $(NAME) binary"

#===================================== RULES ==================================#

all: libft $(NAME)

$(NAME): $(OBJ)
	@for s in $(SUB);\
	do\
		make -sC $$s;\
	done
	@echo "linking..."
	@$(CC) $(LDFLAGS) $^ $(LIBFT) -o $@
	@echo "$(COM_COLOR) $(COM_STRING) $(NO_COLOR)"

-include $(DEP)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c obj
	@printf "compiling: %s\n" $<
	@$(CC) $(CPPFLAGS) $(OPT) $(INC) -MMD -c $< -o $@

obj:
	@mkdir -p $(OBJ_DIR)

clean:
	@for s in $(SUB);\
	do\
		make -sC $$s clean;\
	done
	@rm -rf $(OBJ_DIR)
	@echo "$(COM_COLOR)$(CLEAN_OBJ)$(NO_COLOR)"

fclean: clean
	@for s in $(SUB);\
	do\
		make -sC $$s fclean;\
	done
	@rm -f $(NAME)
	@echo "$(COM_COLOR)$(CLEAN_NAME)$(NO_COLOR)"

re: fclean all

.PHONY: clean fclean all re
