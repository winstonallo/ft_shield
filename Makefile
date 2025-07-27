NAME = ft_shield
PAYLOAD_NAME = server

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc
LIBFT_DIR = libft

BLOCK_SIZE=$(shell stat -fc %s .)

PAYLOAD_SRCS = \
	payload/main.c

SRCS = \
	trojan/main.c

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
PAYLOAD_OBJS = $(addprefix $(OBJ_DIR)/, $(PAYLOAD_SRCS:.c=.o))

HEADERS = $(wildcard $(INC_DIR)/*.h) $(wildcard $(LIBFT_DIR)/src/**/*.h)

LIBFT = $(LIBFT_DIR)/libft.a
LIBFT_FLAGS = -L$(LIBFT_DIR) -lft

CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -I$(LIBFT_DIR)/inc
LDFLAGS = $(LIBFT_FLAGS)

all: $(LIBFT) payload trojan

payload: $(PAYLOAD_OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(PAYLOAD_OBJS) -o $(PAYLOAD_NAME) $(LDFLAGS)

trojan: $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/payload
	mkdir -p $(OBJ_DIR)/trojan

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
