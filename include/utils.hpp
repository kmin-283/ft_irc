#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <list>
# include <queue>
# include <stack>
# include <map>
# include <algorithm>

# include <sstream>
# include <fstream>

# include <sys/socket.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/select.h>
# include <fcntl.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>

# include "Message.hpp"

# define LETTER "abcdefghijklmnopqrstuvwxyzABCDEFGHJKLMNOPQRSTUVWYZ"
# define DIGIT "0123456789"
# define SPECIAL "-[]\\`^{|}"

# define SSL_PORT "6669"
# define ERROR -2
# define ERROR_STR "ERROR"

# define CR_LF "\r\n"

# define CONNECT 1
# define DISCONNECT -1

# define DEFAULT_SIZE 10

# define ERROR_ARG_NUM "Error: Too few or too few arguments."
# define ERROR_PORT "Error: Port must be numeric."
# define ERROR_SOCKET_OPEN_FAIL "Error: Socket open fail."
# define ERROR_GET_PROTO_FAIL "Error: Get protocol fail."
# define ERROR_CONNECT_FAIL "Error: Socket connect fail."
# define ERROR_SELECT_FAIL "Error: Select fail."
# define ERROR_PASS_FAIL "Error: Password invalid."
# define ERROR_SEND_FAIL "Error: Send message fail."
# define ERROR_FILE_OPEN_FAIL "Error: File open fail."

class			Message;

int				ft_atoi(const char *str);
bool			ft_isdigit(char *str);
void			*ft_memset(void *s, int c, size_t n);
bool			isValidFormat(const std::string &key, const char &value);

#endif
