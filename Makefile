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
LDFLAGS			= -fsanitize=address
NAME 			= ft_ping
RM 				= rm -f
RMR				= rm -rf

#-RULES-#

all:			$(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
				$(D_OBJS)
				$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): 		$(OBJS)
				@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBFT_A) -o $(NAME)
				
clean:
				@$(RMR) $(OBJ_DIR)

fclean: 		clean
				@$(RM) $(NAME)

re:				fclean all

.PHONY : 		all clean fclean re bonus