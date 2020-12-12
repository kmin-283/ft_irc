CC = clang++
CFLAGS = -Wall -Werror -Wextra -std=c++98
NAME = ircserv
INCLUDE = -I ./include/

SRC = Server.cpp ServerException.cpp ServerCommand.cpp TLSServer.cpp Client.cpp utils.cpp Message.cpp
SRC_DIR = ./src/
SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJS = $(SRCS:%.cpp=%.o)
SRC_LIB = libirc.a

TEST_NAME = test
TEST = AllTests.cpp MessageParsingTest.cpp
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

$(NAME) : $(SRC_LIB) $(OBJS_M)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS_M) $(SRC_LIB) -o $(NAME)

$(SRC_LIB) : $(OBJS)
	ar rc $(SRC_LIB) $(OBJS)

test : $(SRC_LIB) $(OBJS_T)
	$(CC) $(CFLAGS) $(INCLUDE) $(TEST_H) $(TEST_LIB) $(SRC_LIB) $(OBJS_T) -o $(TEST_NAME)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(TEST_H) -o $@ -c $<

clean:
	rm -rf $(OBJS) $(OBJS_T) $(SRC_LIB) $(OBJS_M)

fclean : clean
	rm -rf $(TEST_NAME) $(NAME) $(DEBUGNAME)

re: fclean all

.PHONY: clean fclean all re
