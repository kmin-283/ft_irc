#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <iostream>

int main()
{
	int flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	char *tmp_port = "80";

	flag = 1;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

printf("here\n");
	if (!(getaddrinfo("www.naver.com", tmp_port, &hints, &addrInfo)))
	{
		printf("error\n");
		return (0);
	}
	printf("out\n");
	addrInfo->ai_addr


}
