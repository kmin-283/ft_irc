#include "Server.hpp"

Server::Server(const char *pass, const char *port)
	: version("ft-irc1.0"), pass(std::string(pass)), info(": kmin seunkim dakim made this server.")
	, port(port), mainSocket(0), maxFd(0), run(true)
{
	FD_ZERO(&this->readFds);
	this->prefix = std::string(":localhost.") + std::string(this->port);

	this->registerCommands();
	this->registerReplies();
	this->initInfo();

	this->serverName = std::string("localhost.") + this->port;
	this->startTime = std::time(NULL);
}

Server::~Server(void)
{}

void					Server::initInfo(void)
{
	std::map<std::string, int (Server::*)(const Message &, Client *)>::iterator it;

	for (it = this->commands.begin(); it != this->commands.end(); ++it)
		this->infos.insert(std::pair<std::string, Info>(it->first, Info()));
}

void					Server::renewFd(const int fd)
{
	FD_SET(fd, &this->readFds);
	if (this->maxFd < fd)
		this->maxFd = fd;
}

void					Server::init(void)
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

void					Server::start(void)
{
	struct timeval timeout;
	clientIter it;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	while(run)
	{
		it = this->acceptClients.begin();
		for (int listenFd = this->mainSocket; listenFd <= this->maxFd; ++it)
		{
			this->renewFd(listenFd);
			if (it == acceptClients.end())
				break ;
			listenFd = it->second.getFd();
		}
		if (ERROR == select(this->maxFd + 1, &this->readFds, NULL, NULL, &timeout))
			std::cout << ERROR_SELECT_FAIL << std::endl;
		it = this->acceptClients.begin();
		for (int listenFd = this->mainSocket; listenFd <= this->maxFd; ++it)
		{
			if (FD_ISSET(listenFd, &this->readFds))
			{
				if (listenFd == this->mainSocket)
					this->connectClient();
				else
					this->receiveMessage(listenFd);
			}
			if (it == acceptClients.end() || !run)
				break ;
			listenFd = it->second.getFd();
		}
	}
}

void					Server::connectClient(void)
{
	int					newFd;
	struct sockaddr_in6	remoteAddress;
	socklen_t			addressLen;

	addressLen = sizeof(struct sockaddr_in);
	if (ERROR == (newFd = accept(this->mainSocket, (struct sockaddr*)&remoteAddress, &addressLen)))
		throw Server::AcceptFailException();
	fcntl(newFd, F_SETFL, O_NONBLOCK);
	this->renewFd(newFd);
	this->acceptClients.insert(std::pair<int, Client>(newFd, Client(newFd)));
	std::cout << "Connect client." << std::endl;
}

void					Server::receiveMessage(const int fd)
{
	char		buffer;
	int			readResult;
	std::string	messageStr;
	int			connectionStatus;
	Client 		&sender = this->acceptClients[fd];
	Message		sendMessage;

	messageStr = "";
	readResult = 0;
	connectionStatus = CONNECT;

	while (0 < (readResult = recv(sender.getFd(), &buffer, 1, 0)))
	{
		messageStr += buffer;
		if (buffer == '\n')
		{
			sendMessage = Message(messageStr);
			std::cout << "Reveive message = " << sendMessage.getTotalMessage();
			if (this->commands.find(sendMessage.getCommand()) != this->commands.end())
			{
				if (sender.getStatus() == USER)
				{
					sender.incrementQueryData(SENDMSG, 1);
					sender.incrementQueryData(SENDBYTES, sendMessage.getTotalMessage().length());
				}
				else if (sender.getStatus() == SERVER)
				{
					sender.incrementQueryData(SENDMSG, 1);
					sender.incrementQueryData(SENDBYTES, sendMessage.getTotalMessage().length());
				}
				connectionStatus = (this->*(this->commands[sendMessage.getCommand()]))(sendMessage, &sender);
			}
			messageStr.clear();
		}
		if (connectionStatus == DISCONNECT || connectionStatus == TOTALDISCONNECT)
			break ;
	}
	if (readResult == 0)
		this->disconnectClient(sendMessage, &sender);
	if (connectionStatus == TOTALDISCONNECT)
		this->clearClient(&sender);
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
	if (addrInfo == NULL)
		std::cout << "addrinfo NULL" << std::endl;
	return (addrInfo);
}

void					Server::connectServer(std::string address)
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
			if (connect(newFd, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen) < 0)
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
	std::string password = address.substr(address.rfind(":") + 1, address.length() - 1);
	Message passMessage("PASS " + password + CR_LF);
	Message serverMessage("SERVER " + this->serverName + " 1 " + this->info + CR_LF); //토큰 추가
	this->sendMessage(passMessage, &this->acceptClients[newFd]);
	this->sendMessage(serverMessage, &this->acceptClients[newFd]);
	std::cout << "Connect other server." << std::endl;


	// Message versionMessage(std::string(":localhost.3000 VERSION irc.example.net")+ CR_LF);
	// this->sendMessage(versionMessage, &newClient);
	// std::cout << "versionnn" << std::endl;
}

void					Server::clearClient(Client *client)
{
	close(client->getFd());
	FD_CLR(client->getFd(), &this->readFds);
	this->sendClients.clear();
	this->serverList.clear();
	this->clientList.clear();
	this->acceptClients.clear();
}

void				Server::getChildServer(std::list<std::string> &serverList, std::string key)
{
	std::map<std::string, Client>::iterator	iterator;

	for(iterator = this->sendClients.begin(); iterator != this->sendClients.end(); ++iterator)
	{
		if (iterator->second.getInfo(UPLINKSERVER) == key)
		{
			if (iterator->second.getStatus() == SERVER)
				this->getChildServer(serverList, iterator->second.getInfo(SERVERNAME));
			serverList.push_back(iterator->second.getInfo(SERVERNAME));
		}
	}
}

void					Server::disconnectChild(const Message &message, Client *client)
{
	std::list<std::string>::iterator	iterator;
	std::list<std::string>				serverList;

	this->getChildServer(serverList, client->getInfo(SERVERNAME));
	for(iterator = serverList.begin(); iterator != serverList.end(); ++iterator)
	{
		if (this->sendClients[*iterator].getStatus() == SERVER)
		{
			this->sendClients[*iterator].setInfo(UPLINKSERVER, client->getInfo(SERVERNAME));
			(this->*(this->replies[RPL_SQUITBROADCAST]))(message, &this->sendClients[*iterator]);
		}
		this->sendClients.erase(*iterator);
	}
}

void					Server::disconnectClient(const Message &message, Client *client)
{
	std::string stringKey;

	stringKey = client->getInfo(NICK);
	if (this->acceptClients.count(client->getFd()))
	{
		if (this->clientList.count(stringKey))
		{
			(this->*(this->replies[RPL_QUITBROADCAST]))(message, client);
			this->clientList.erase(stringKey);
		}
		else if (this->serverList.count(stringKey))
		{
			this->disconnectChild(message, client);
			(this->*(this->replies[RPL_SQUITBROADCAST]))(message, client);
			this->serverList.erase(stringKey);
		}
		close(client->getFd());
		FD_CLR(client->getFd(), &this->readFds);
		this->acceptClients.erase(client->getFd());
	}
	if (this->sendClients.count(stringKey))
		this->sendClients.erase(stringKey);
}

void					Server::sendMessage(const Message &message, Client *client)
{
	//TODO 512자가 넘은 경우 나누어 전송해야함

	if (client->getStatus() == USER)
	{
		client->incrementQueryData(RECVMSG, 1);
		client->incrementQueryData(RECVBYTES, message.getTotalMessage().length());
	}
	else if (client->getStatus() == SERVER)
	{
		client->incrementQueryData(RECVMSG, 1);
		client->incrementQueryData(RECVBYTES, message.getTotalMessage().length());
	}
	if (ERROR == write(client->getFd(), message.getTotalMessage().c_str(), message.getTotalMessage().length()))
		std::cerr << ERROR_SEND_FAIL << std::endl;
}

void					Server::broadcastMessage(const Message &message, Client *client)
{
	std::map<std::string, Client*>::iterator	iterator;

	for (iterator = this->serverList.begin(); iterator != this->serverList.end(); ++iterator)
	{
		if (iterator->second->getInfo(SERVERNAME) != client->getInfo(SERVERNAME))
		{
			this->sendMessage(message, iterator->second);
		}
	}
}
