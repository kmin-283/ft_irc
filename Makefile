CC = clang++
CFLAGS = -g3 -fsanitize=address -Wall -Werror -Wextra
NAME = ircserv
INCLUDE = -I ./include/

SRC = main.cpp Server.cpp ServerException.cpp Client.cpp utils.cpp Message.cpp
SRC_DIR = ./src/
SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJS = $(SRCS:%.cpp=%.o)
SRC_LIB = libirc.a

SRC_COM = nickHandler.cpp passHandler.cpp serverHandler.cpp userHandler.cpp
SRC_COM_DIR = ./src/commandHandler/
SRCS_COM = $(addprefix $(SRC_COM_DIR), $(SRC_COM))
OBJS_COM = $(SRCS_COM:%.cpp=%.o)


all : $(NAME)

$(NAME) : $(OBJS) $(OBJS_COM)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(OBJS_COM) -o $(NAME)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(TEST_H) -o $@ -c $<

clean:
	rm -rf $(OBJS) $(OBJS_COM)

fclean : clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean all re
