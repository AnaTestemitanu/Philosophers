NAME	= philo
CC	  = gcc
CFLAGS  = -Werror -Wall -Wextra -pthread

SRC_PATH = srcs/
OBJ_PATH = objects/

SRC	 = main.c \
	input_validation.c \
	grim_reaper.c \
	time_management.c \
	philosopher_routines.c \
	table_initialization.c \
	output.c \
	cleanup.c
SRCS	= $(addprefix $(SRC_PATH), $(SRC))
OBJS	= $(patsubst $(SRC_PATH)%.c, $(OBJ_PATH)%.o, $(SRCS))

INC	 = -I includes/

all: $(NAME)

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c | $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean
