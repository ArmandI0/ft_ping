#-PATH FILES-#

SRC_DIR			= src
OBJ_DIR			= obj

SRCS			= \
					main.c \
				
SRC				= $(addprefix src/, $(SRCS))
OBJS			= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
D_OBJS			= mkdir -p $(@D)

#-UTILS-#

CC 				= cc
CFLAGS 			= -Wall -Wextra -Werror
NAME 			= ft_ping
RM 				= rm -f
RMR				= rm -rf

#-RULES-#

all:			$(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
				$(D_OBJS)
				$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): 		$(OBJS)
				@$(CC) $(CFLAGS) $(OBJS) $(LIBFT_A) -o $(NAME)
				
clean:
				@$(RMR) $(OBJ_DIR)
				@$(RMR) $(OBJ_BONUS_DIR)
				@$(RMR) $(LIBMLX)/build
				@make -C $(LIBFT) clean

fclean: 		clean
				@$(RM) $(NAME)
				@$(RM) $(NAME_BONUS)
				@make -C $(LIBFT) fclean

re:				fclean all

bonus :			$(LIBFT_A) $(LIBMLX_A) $(OBJS_BONUS)
				@$(RM) $(NAME)
				@$(CC) $(CFLAGS) $(OBJS_BONUS) $(LIBFT_A)  $(LIBMLX_A) -ldl -lglfw -pthread -lm $(HEADERS) -o $(NAME)

.PHONY : 		all clean fclean re bonus