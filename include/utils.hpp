#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <list>
# include <queue>
# include <stack>
# include <map>
# include <set>
# include <algorithm>
# include <cctype>
# include <ctime>

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
# define USER_FORMAT "\r\n @"
# define MODEUSER "aiwroOs"
# define MODECHANNEL "oOvaimnqpsrtklber"

# define SSL_PORT "6669"
# define ERROR -1
# define ERROR_STR "ERROR"

# define CR_LF "\r\n"

# define CONNECT 1
# define DISCONNECT 0
# define TOTALDISCONNECT -2

# define DEFAULT_SIZE 10
# define NUM_OF_COMMANDS 30


# define TIMEOUT "T"
# define PING_LIMIT 150
# define PING_CYCLE 150

# define ON true
# define OFF false

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

bool 			match(const char *first, const char *second);
int				ft_atoi(const char *str);
bool			isInTheMask(const std::string &mask, const char &target);
void			*ft_memset(void *s, int c, size_t n);
std::string		getTimestamp(const std::time_t &startTime, const bool &forUptime);
bool            isValidPort(const std::string &port);
bool            isValidIpv4(const std::string &ipAddress);
bool            isValidAddress(const std::string &address);

#endif
