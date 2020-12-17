#ifndef FT_HPP_HPP
# define FT_HPP_HPP

# include "CppUTest/TestHarness.h"
# include "Message.hpp"
# include "Client.hpp"
# include <unistd.h>

# define private public
# include "Server.hpp"
# undef private

int			get_next_line(int fd, char **line);

#endif