# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jbeall <jbeall@student.42.us.org>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/03/03 12:06:08 by jbeall            #+#    #+#              #
#    Updated: 2019/07/15 21:02:40 by jbeall           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#==================================== GENERAL =================================#

NAME1 = server
NAME2 = client
CC = clang
CPPFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -pipe -flto=full #-fsanitize=address,undefined
# OPT = -O3 -flto=full -march=native #-fsanitize=address,undefined
SUB = libft

#=================================== SOURCES ==================================#

VPATH = src/server src/client src

LIST1 = server
SRC1 = $(addsuffix .c, $(LIST1))

LIST2 = client
SRC2 = $(addsuffix .c, $(LIST2))

LISTC = utils
SRCC = $(addsuffix .c, $(LISTC))

#=================================== OBJECTS ==================================#

OBJ_DIR = obj/
OBJ1 = $(addprefix $(OBJ_DIR), $(SRC1:.c=.o))
DEP1 = $(OBJ1:%.o=%.d)

OBJ2 = $(addprefix $(OBJ_DIR), $(SRC2:.c=.o))
DEP2 = $(OBJ2:%.o=%.d)

OBJC = $(addprefix $(OBJ_DIR), $(SRCC:.c=.o))
DEPC = $(OBJC:%.o=%.d)

#================================== LIBRARIES =================================#

LIBFT = -L ./libft/ -lft

#=================================== HEADERS ==================================#

INC_DIR = ./include/
LIB_INC = libft/includes/
INC = -I $(LIB_INC) -I $(INC_DIR)

#==================================== COLORS ==================================#

#COLORS
COM_COLOR   = \033[92m
NO_COLOR    = \033[m

COM_STRING  = "$(NAME) compilation successful"
CLEAN_OBJ	= "cleaned $(NAME) objects"
CLEAN_NAME	= "cleaned $(NAME) binary"

#===================================== RULES ==================================#

all: libft $(NAME1) $(NAME2)

$(NAME1): $(OBJ1) $(OBJC)
	@for s in $(SUB);\
	do\
		make -sC $$s;\
	done
	@echo "linking..."
	@$(CC) $(LDFLAGS) $^ $(LIBFT) -o $@
	@echo "$(COM_COLOR) $(COM_STRING) $(NO_COLOR)"

$(NAME2): $(OBJ2) $(OBJC)
	@for s in $(SUB);\
	do\
		make -sC $$s;\
	done
	@echo "linking..."
	@$(CC) $(LDFLAGS) $^ $(LIBFT) -o $@
	@echo "$(COM_COLOR) $(COM_STRING) $(NO_COLOR)"

-include $(DEP1)
-include $(DEP2)
-include $(DEPC)

$(OBJ_DIR)%.o: %.c | obj
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
	@rm -f $(NAME1)
	@rm -f $(NAME2)
	@echo "$(COM_COLOR)$(CLEAN_NAME)$(NO_COLOR)"

re: fclean all

.PHONY: clean fclean all re
