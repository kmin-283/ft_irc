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

# define SSL_PORT "6669"
# define CONNECT_FAIL -1

# define ERROR_ARG_NUM "Error: Too few or too few arguments."
# define ERROR_PORT "Error: Port must be numeric."
# define ERROR_SOCKET_OPEN_FAIL "Error: Socket open fail."
# define ERROR_GET_PROTO_FAIL "Error: Get protocol fail."
# define ERROR_CONNECT_FAIL "Error: Socket connect fail."
# define ERROR_SELECT_FAIL "Error: Select fail."


int				ft_atoi(char *str);
bool			ft_isdigit(char *str);
void			*ft_memset(void *s, int c, size_t n);

#endif
