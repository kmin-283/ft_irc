#include "Server.hpp"


Server::Server(const char *pass, const char *port)
	: pass(std::string(pass)), port(port), mainSocket(0), maxFd(0)
{
	FD_ZERO(&this->readFds);
	this->prefix = std::string(":localhost.") + std::string(this->port);
	this->commands["PASS"] = &Server::passHandler;
	this->commands["NICK"] = &Server::nickHandler;
	this->commands["USER"] = &Server::userHandler;

}

Server::~Server(void)
{
	this->clearClient();
}

std::string		Server::getPass(void) const
{
	return (this->pass);
}

int				Server::getSocket(void) const
{
	return (this->mainSocket);
}

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
	Client *newClient = new Client(newFd);
	this->acceptClients.insert(std::pair<int, Client*>(newFd, newClient));
	std::cout << "connection success " << std::endl;
}

void			Server::receiveMessage(const int fd)
{
	char		buffer;
	int			readResult;
	std::string	messageStr;
	Client 		*sender;

	messageStr = "";
	readResult = 0;
	sender = this->acceptClients[fd];
	while (0 < (readResult = recv(sender->getFd(), &buffer, 1, 0)))
	{
		messageStr += buffer;
		if (buffer == '\n')
		{
			Message message(messageStr);
			if (this->commands.find(message.getCommand()) != this->commands.end())
				(this->*(this->commands[message.getCommand()]))(message, *sender);
			messageStr = "";
		}
	}
	if (readResult == -1 && errno != EAGAIN)
	{
		// TODO에러메시지
		// close(sender->getFd());
		throw Server::ReceiveMessageFailException();
	}
	if (readResult == 0)
	{
		std::cout << "connection fail" << std::endl;
		close(sender->getFd());
		this->acceptClients.erase(sender->getFd());
		// TODO sendClients map에서 삭제할 필요 있음
		FD_CLR(sender->getFd(), &this->readFds);
		delete sender;
		// TODO 테스트 필요
	}
}

struct addrinfo	*Server::getAddrInfo(const std::string info)
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
	addrInfo = this->getAddrInfo(address);
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
	Client *newClient = new Client(newFd);
	this->acceptClients.insert(std::pair<int, Client*>(newFd, newClient));

	// TODO 서버 등록관련 커멘드 전송 필요
	// PASS 123 /r/n SERVER 12312
	std::cout << "send" << std::endl;
	char buffer[100] ="PASS my\r\n";
	send(newClient->getFd(), buffer, 9, 0);
	char buffer1[100] ="SERVER localhost.6670 1 :2123\r\n";
	send(newClient->getFd(), buffer1, 31, 0);

	std::string password;

	password = address.substr(address.rfind(":") + 1, address.length() - 1);
	// std::cout << "password = " << password << std::endl;
}

void			Server::clearClient(void)
{
	std::map<int, Client*>::iterator acceptIter = this->acceptClients.begin();
	std::map<std::string, Client*>::iterator senderIter = this->sendClients.begin();

	for (;acceptIter != this->acceptClients.end(); acceptIter++)
		delete acceptIter->second;
	for (;senderIter != this->sendClients.end(); senderIter++)
		delete senderIter->second;
}
