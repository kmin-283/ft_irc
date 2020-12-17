#ifndef FT_HPP_HPP
# define FT_HPP_HPP

# include "CppUTest/TestHarness.h"
# include "Message.hpp"
# include "Client.hpp"

# define private public
# include "Server.hpp"
# undef private

extern "C"
{
	#include <unistd.h>
	int			get_next_line(int fd, char **line);
}

#endif