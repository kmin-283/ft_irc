CC = clang++
CFLAGS = -g3 -fsanitize=address -Wall -Werror -Wextra
NAME = ircserv
INCLUDE = -I ./include/

SRC = main.cpp Server.cpp ServerException.cpp Client.cpp utils.cpp Message.cpp
SRC_DIR = ./src/
SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJS = $(SRCS:%.cpp=%.o)
SRC_LIB = libirc.a

SRC_COM = registerCommands.cpp passHandler.cpp userHandler.cpp serverHandler.cpp
SRC_COM_DIR = ./src/commandHandler/
SRCS_COM = $(addprefix $(SRC_COM_DIR), $(SRC_COM))
OBJS_COM = $(SRCS_COM:%.cpp=%.o)

SRC_RE = registerReplies.cpp commandResponse.cpp errorResponse.cpp
SRC_RE_DIR = ./src/responseHandler/
SRCS_RE = $(addprefix $(SRC_RE_DIR), $(SRC_RE))
OBJS_RE = $(SRCS_RE:%.cpp=%.o)

all : $(NAME)

$(NAME) : $(OBJS) $(OBJS_COM) $(OBJS_RE)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $(OBJS_COM) $(OBJS_RE) -o $(NAME)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

test:
	cd ./tests; make
	./tests/test

clean:
	rm -rf $(OBJS) $(OBJS_COM) $(OBJS_RE)

fclean : clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean all re
