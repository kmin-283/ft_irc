#include "Server.hpp"

Server::Server(const char *pass)
	: pass(std::string(pass)), mainSocket(0), maxFd(0)
{
	FD_ZERO(&this->readFds);
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

void			Server::init(const char *port)
{
	// int				flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	struct addrinfo	*addrInfoIterator;

	// flag = 1;
	ft_memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(NULL, port, &hints, &addrInfo) != 0)
		throw Server::GetAddressFailException();
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next)
	{
		if (addrInfoIterator->ai_family == AF_INET6)
		{
			if (CONNECT_FAIL == (this->mainSocket = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			// setsockopt(this->mainSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
			if (CONNECT_FAIL == (bind(this->mainSocket, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen)))
				throw Server::SocketBindFailException();
			if (CONNECT_FAIL == (listen(this->mainSocket, SOMAXCONN)))
				throw Server::SocketListenFailException();
		}
	}
	this->renewFd(this->mainSocket);
	freeaddrinfo(addrInfo);
}

void			Server::start(void)
{
	while(42)
	{
		if (CONNECT_FAIL == select(this->maxFd + 1, &this->readFds, NULL, NULL, NULL))
			throw Server::SelectFailException();
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
	if (CONNECT_FAIL == (newFd = accept(this->mainSocket, (struct sockaddr*)&remoteAddress, &addressLen)))
		throw Server::AcceptFailException();
	fcntl(newFd, F_SETFL, O_NONBLOCK);
	this->renewFd(newFd);
	Client *newClient = new Client(newFd);
	this->acceptClients.insert(std::pair<int, Client*>(newFd, newClient));
	std::cout << "accept connection success" << std::endl;
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
		if (2 <= messageStr.length() && "\r\n" == messageStr.substr(messageStr.length() - 2, messageStr.length()))
		{
			Message message(messageStr);
			// std::cout << message.getPrefix() << " " << message.getCommand() << " " <<  message.getParameter() << std::endl;
			// this->command()(message, client);
			messageStr = "";
		}
	}
	if (readResult == -1 && errno != EAGAIN)
	{
		// TODO에러메시지
		throw Server::ReceiveMessageFailException();
	}
	else if (readResult == 0)
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

struct addrinfo	*Server::getAddrInfo(std::string info)
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
			if (CONNECT_FAIL == (newFd = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			if (CONNECT_FAIL == connect(newFd, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen))
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
	char buffer[100] ="SERVER server 1 1 :2123\r\n";
	send(newClient->getFd(), buffer, 25, 0);

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