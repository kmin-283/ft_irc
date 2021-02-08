cmake_minimum_required(VERSION 3.17)
project(ft_irc)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(include)
include_directories(res)
include_directories(src)
include_directories(tests)
include_directories(tests/gnl)

add_executable(ft_irc
        include/Channel.hpp
        include/Client.hpp
        include/Message.hpp
        include/Replies.hpp
        include/Server.hpp
        include/utils.hpp
        src/commandHandler/channelHandler.cpp
        src/commandHandler/infoHandler.cpp
        src/commandHandler/passHandler.cpp
        src/commandHandler/privmsgHandler.cpp
        src/commandHandler/registerCommands.cpp
        src/commandHandler/serverHandler.cpp
        src/commandHandler/userHandler.cpp
        src/commandHandler/wallopsHandler.cpp
        src/responseHandler/commandResponse.cpp
        src/responseHandler/errorResponse.cpp
        src/responseHandler/registerReplies.cpp
        src/Channel.cpp
        src/Client.cpp
        src/Info.cpp
        src/main.cpp
        src/Message.cpp
        src/Server.cpp
        src/ServerException.cpp
        src/utils.cpp
        tests/gnl/get_next_line.c
        tests/gnl/get_next_line.h
        tests/gnl/get_next_line_utils.c
        tests/AllTests.cpp
        tests/disconnectClientTest.cpp
        tests/messageConstructTest.cpp
        tests/messageParsingTest.cpp
        tests/nickHandlerTest.cpp
        tests/quitHandlerTest.cpp
        tests/registerUserTest.cpp
        tests/serverHandlerTest.cpp
        tests/squitHandlerTest.cpp
        tests/Test.hpp
        tests/userHandlerTest.cpp
        Makefile)
