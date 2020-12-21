#ifndef TEST_HPP
# define define TEST_HPP

# include "CppUTest/TestHarness.h"

# define private public
# include "Server.hpp"
# undef private

extern "C"
{
	#include <unistd.h>
	int			get_next_line(int fd, char **line);
}

#endif
