#include "Server.hpp"


Server::Server(const char *pass, const char *port)
	: pass(std::string(pass)), port(port), mainSocket(0), maxFd(0)
{
	FD_ZERO(&this->readFds);
	this->prefix = std::string(":localhost.") + std::string(this->port);
	this->commands["PASS"] = &Server::passHandler;
	this->commands["NICK"] = &Server::nickHandler;
	this->commands["USER"] = &Server::userHandler;
	this->commands["SERVER"] = &Server::serverHandler;
}

Server::~Server(void)
{}

void			Server::renewFd(const int fd)
{
	FD_SET(fd, &this->readFds);
	if (this->maxFd < fd)
		this->maxFd = fd;
}

void			Server::init(void)
{
	int				flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	struct addrinfo	*addrInfoIterator;

	flag = 1;
	ft_memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(NULL, this->port, &hints, &addrInfo) != 0)
		throw Server::GetAddressFailException();
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next)
	{
		if (addrInfoIterator->ai_family == AF_INET6)
		{
			if (ERROR == (this->mainSocket = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			setsockopt(this->mainSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
			if (ERROR == (bind(this->mainSocket, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen)))
				throw Server::SocketBindFailException();
			if (ERROR == (listen(this->mainSocket, SOMAXCONN)))
				throw Server::SocketListenFailException();
		}
	}
	this->renewFd(this->mainSocket);
	freeaddrinfo(addrInfo);
}

void			Server::start(void)
{
	struct timeval timeout;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	while(42)
	{
		for (int listenFd = this->mainSocket; listenFd <= this->maxFd; ++listenFd)
			this->renewFd(listenFd);
		if (ERROR == select(this->maxFd + 1, &this->readFds, NULL, NULL, &timeout))
			std::cerr << ERROR_SELECT_FAIL << std::endl;
		for (int listenFd = this->mainSocket; listenFd <= this->maxFd; ++listenFd)
		{
			if (FD_ISSET(listenFd, &this->readFds))
			{
				std::cout << "fd_isset" << std::endl;
				if (listenFd == this->mainSocket)
					this->acceptConnection();
				else
					this->receiveMessage(listenFd);
			}
		}
	}
}

void			Server::acceptConnection(void)
{
	int					newFd;
	struct sockaddr_in6	remoteAddress;
	socklen_t			addressLen;

	addressLen = sizeof(struct sockaddr_in);
	if (ERROR == (newFd = accept(this->mainSocket, (struct sockaddr*)&remoteAddress, &addressLen)))
		throw Server::AcceptFailException();
	fcntl(newFd, F_SETFL, O_NONBLOCK);
	this->renewFd(newFd);
	this->acceptClients.insert(std::pair<int, Client>(newFd, Client(newFd, false)));
	std::cout << "accept connection" << std::endl;
}

void			Server::receiveMessage(const int fd)
{
	char		buffer;
	int			readResult;
	std::string	messageStr;
	int			connectionStatus;
	Client 		&sender = this->acceptClients[fd];

	messageStr = "";
	readResult = 0;
	connectionStatus = CONNECT;
	while (0 < (readResult = recv(sender.getFd(), &buffer, 1, 0)))
	{
		messageStr += buffer;
		if (buffer == '\n')
		{
			Message message(messageStr);
			std::cout << messageStr << std::endl;
			if (this->commands.find(message.getCommand()) != this->commands.end())
				connectionStatus = (this->*(this->commands[message.getCommand()]))(message, &sender);
			messageStr = "";
		}
		if (connectionStatus == DISCONNECT)
			break ;
	}
	if (connectionStatus == DISCONNECT || readResult == 0)
		this->disconnectClient(&sender);
}

static struct addrinfo	*getAddrInfo(const std::string info)
{
	int				i;
	int				j;
	std::string		port;
	std::string		address;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;

	// TODO address가 제대로 들어왔는지 체크하는 함수필요 할 수 있음
	i = info.rfind(":");
	j = info.rfind(":", i - 1);
	port = info.substr(j + 1, i - j - 1);
	address = info.substr(0, j);
	ft_memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(address.c_str(), port.c_str(), &hints, &addrInfo) != 0)
		throw Server::GetAddressFailException();
	return (addrInfo);
}

void			Server::connectServer(std::string address)
{
	int					newFd;
	int					ipVersion;
	struct addrinfo		*addrInfo;
	struct addrinfo		*addrInfoIterator;

	ipVersion = AF_INET;
	addrInfo = getAddrInfo(address);
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next)
	{
		if (addrInfoIterator->ai_family == AF_INET6)
			ipVersion = AF_INET6;
	}
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next)
	{
		if (addrInfoIterator->ai_family == ipVersion)
		{
			if (ERROR == (newFd = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			if (ERROR == connect(newFd, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen))
			{
				close(newFd);
				std::cerr << ERROR_CONNECT_FAIL << std::endl;
				freeaddrinfo(addrInfo);
				return ;
			}
		}
	}
	freeaddrinfo(addrInfo);
	fcntl(newFd, F_SETFL, O_NONBLOCK);
	this->renewFd(newFd);
	Client newClient(newFd);
	this->acceptClients.insert(std::pair<int, Client>(newFd, newClient));

	// TODO 서버 등록관련 커멘드 전송 필요
	std::cout << "send" << std::endl;
	char buffer[100] ="PASS my\r\n";
	send(newClient.getFd(), buffer, 9, 0);

	std::string sn("SERVER ");
	sn += this->prefix.substr(1, this->prefix.length());
	sn += " 1 :123\r\n";
	send(newClient.getFd(), sn.c_str(), sn.length(), 0);
	std::string password;

	password = address.substr(address.rfind(":") + 1, address.length() - 1);
}

void			Server::disconnectClient(Client *client)
{
	std::cout << "disconnect " << std::endl;
	close(client->getFd());
	FD_CLR(client->getFd(), &this->readFds);
	if (this->acceptClients.find(client->getFd()) != this->acceptClients.end())
	this->acceptClients.erase(client->getFd());
	// if (this->sendClients.find(client->getCurrentNick()) != this->sendClients.end())
	// 	this->sendClients.erase(client->getCurrentNick());
	// if (this->serverList.find(client->getCurrentNick()))
	// TODO sendClients map에서 삭제할 필요 있음
	
}

void				Server::sendMessage(const Message &message, Client *client)
{
	if (ERROR == send(client->getFd(), message.getTotalMessage().c_str(), message.getTotalMessage().length(), 0))
		std::cerr << ERROR_SEND_FAIL << std::endl;
}
