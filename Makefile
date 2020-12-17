CC = clang++
CFLAGS = -g3 -fsanitize=address -Wall -Werror -Wextra -std=c++98
NAME = ircserv
INCLUDE = -I ./include/

SRC = Server.cpp ServerException.cpp TLSServer.cpp Client.cpp utils.cpp Message.cpp
SRC_DIR = ./src/
SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJS = $(SRCS:%.cpp=%.o)
SRC_LIB = libirc.a

SRC_COM = nickHandler.cpp passHandler.cpp serverHandler.cpp userHandler.cpp
SRC_COM_DIR = ./src/commandHandler/
SRCS_COM = $(addprefix $(SRC_COM_DIR), $(SRC_COM))
OBJS_COM = $(SRCS_COM:%.cpp=%.o)

TEST_NAME = test
TEST = AllTests.cpp MessageParsingTest.cpp MessageConstructTest.cpp ServerHandlerTest.cpp
TEST_DIR = ./tests/
TESTS = $(addprefix $(TEST_DIR), $(TEST))
OBJS_T = $(TESTS:%.cpp=%.o)
TEST_H = -I /usr/local/include/
# TEST_H = -I ~/.brew/include
# TEST_LIB = -L ~/.brew/lib -lCppUTest -lCppUTestExt
TEST_LIB = -lCppUTest -lCppUTestExt

MAIN = main.cpp
OBJS_M = $(MAIN:%.cpp=%.o)

all : $(NAME)

$(NAME) : $(SRC_LIB) $(OBJS_M) $(OBJS_COM)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS_M) $(OBJS_COM) $(SRC_LIB) -o $(NAME)

$(SRC_LIB) : $(OBJS)
	ar rc $(SRC_LIB) $(OBJS)

test : $(SRC_LIB) $(OBJS_T) $(OBJS_COM)
	cd ./tests/gnl; make all
	cp ./tests/gnl/libgnl.a ./
	$(CC) $(CFLAGS) $(INCLUDE) $(TEST_H) $(TEST_LIB) $(SRC_LIB) $(OBJS_T) libgnl.a -o $(TEST_NAME)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(TEST_H) -o $@ -c $<

clean:
	cd ./tests/gnl; make fclean
	rm -rf $(OBJS) $(OBJS_T) $(SRC_LIB) $(OBJS_M) $(OBJS_COM) libgnl.a

fclean : clean
	rm -rf $(TEST_NAME) $(NAME) $(DEBUGNAME)

re: fclean all

.PHONY: clean fclean all re
