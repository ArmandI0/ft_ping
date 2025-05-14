#-PATH FILES-#

SRC_DIR			= src
OBJ_DIR			= obj

SRCS			= \
				main.c \
				parsing.c \
				socket.c \
				struct-management.c \
				utils.c \
				signal.c \
				packet-statistics.c \
				print_result.c \
				
SRC				= $(addprefix src/, $(SRCS))
OBJS			= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
D_OBJS			= mkdir -p $(@D)

#-UTILS-#

CC 				= cc
CFLAGS 			= -Wall -Wextra -Werror -g 
NAME 			= ft_ping
RM 				= rm -f
RMR				= rm -rf

#-RULES-#

all:			$(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
				$(D_OBJS)
				$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): 		$(OBJS)
				@$(CC) $(CFLAGS) $(OBJS) $(LIBFT_A) -o $(NAME) -lm
				
clean:
				@$(RMR) $(OBJ_DIR)

fclean: 		clean
				@$(RM) $(NAME)

re:				fclean all

.PHONY : 		all clean fclean re bonus