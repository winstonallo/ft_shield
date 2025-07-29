NAME = ft_shield

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc
LIBFT_DIR = libft

BLOCK_SIZE=$(shell stat -fc %s .)

PAYLOAD_SRCS = payload/main.c
SRCS = \
	main.c \
	server.c \
	log.c 

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

DEBUG_OBJS = $(addprefix $(OBJ_DIR)/debug/, $(SRCS:.c=.o))

HEADERS = $(wildcard $(INC_DIR)/*.h) $(wildcard $(LIBFT_DIR)/src/**/*.h)
LIBFT = $(LIBFT_DIR)/libft.a
LIBFT_FLAGS = -L$(LIBFT_DIR) -lft

CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -I$(LIBFT_DIR)/inc -DBLOCK_SIZE=$(BLOCK_SIZE)
DEBUG_FLAGS = -DDEBUG=1 -g
LDFLAGS = $(LIBFT_FLAGS)

all: $(LIBFT) $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	strip $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c $(HEADERS) | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/debug/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

$(OBJ_DIR)/debug/%.o: %.c $(HEADERS) | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(PAYLOAD_NAME)

re: fclean all

.PHONY: all debug clean fclean re debug-payload debug-trojan payload trojan
