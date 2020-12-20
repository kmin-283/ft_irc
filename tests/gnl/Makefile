CC = gcc
CFLAGS = -Wall -Wextra -Werror
NAME = libgnl.a
SRCS = get_next_line.c get_next_line_utils.c
BUFFER = -D BUFFER_SIZE=10
OBJS = $(SRCS:%.c=%.o)

all: $(NAME)

$(NAME): $(OBJS)
		ar rc $(NAME) $(OBJS)

.c.o: $(SRCS)
	$(CC) $(CFLAGS) $(BUFFER) -o $@ -c $<

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)


re: clean all

.PHONY: clean fclean all re
