CC = clang++
CFLAGS = -fsanitize=address -g -Wall -Werror -Wextra
NAME = test
INCLUDE = -I ../include/

SRC = Client.cpp Message.cpp Server.cpp ServerException.cpp utils.cpp Info.cpp
SRC_DIR = ../src/
SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJS = $(SRCS:%.cpp=%.o)
SRC_LIB = libirc.a

SRC_COM = registerCommands.cpp passHandler.cpp userHandler.cpp serverHandler.cpp wallopsHandler.cpp infoHandler.cpp
SRC_COM_DIR = ../src/commandHandler/
SRCS_COM = $(addprefix $(SRC_COM_DIR), $(SRC_COM))
OBJS_COM = $(SRCS_COM:%.cpp=%.o)

SRC_RE = registerReplies.cpp commandResponse.cpp errorResponse.cpp
SRC_RE_DIR = ../src/responseHandler/
SRCS_RE = $(addprefix $(SRC_RE_DIR), $(SRC_RE))
OBJS_RE = $(SRCS_RE:%.cpp=%.o)

TEST = AllTests.cpp nickHandlerTest.cpp userHandlerTest.cpp registerUserTest.cpp serverHandlerTest.cpp disconnectClientTest.cpp squitHandlerTest.cpp messageConstructTest.cpp messageParsingTest.cpp quitHandlerTest.cpp
OBJS_T = $(TEST:%.cpp=%.o)
TEST_H = -I /usr/local/Cellar/cpputest/4.0/include
# TEST_H = -I ~/.brew/include
# TEST_LIB = -L ~/.brew/lib -lCppUTest -lCppUTestExt
TEST_LIB = -L /usr/local/Cellar/cpputest/4.0/lib -lCppUTest -lCppUTestExt

all : $(NAME)

$(NAME) : $(SRC_LIB) $(OBJS_T)
	$(CC) $(CFLAGS) $(INCLUDE) $(TEST_H) $(SRC_LIB) $(TEST_LIB) $(OBJS_T) -o $(NAME)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(TEST_H) -o $@ -c $<

$(SRC_LIB) : $(OBJS) $(OBJS_COM) $(OBJS_RE)
	cd ./gnl; make
	mv ./gnl/libgnl.a $(SRC_LIB)
	ar rc $(SRC_LIB) $(OBJS) $(OBJS_COM) $(OBJS_RE)

clean:
	cd ./gnl; make fclean
	cd ../; make fclean
	rm -rf $(OBJS) $(OBJS_COM) $(OBJS_COM) $(OBJS_T) $(SRC_LIB)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean all re

