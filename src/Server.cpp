#include "Server.hpp"

Server::Server(const char *pass, const char *port)
	: ipAddress("127.0.0.1"), version("ft-irc1.0"), pass(std::string(pass)), info(":kmin seunkim dakim made this server.")
	  , port(port), mainSocket(0), maxFd(0), run(true), adminLoc1("kmin"), adminLoc2("Seoul"), adminEmail("admin@admin.irc")
	  , ctx(NULL), isSSL(false)
{
	this->tlsPort = std::to_string(ft_atoi(port) + 1);
	FD_ZERO(&this->readFds);
	this->prefix = std::string(":localhost.") + std::string(this->port);

	this->registerCommands();
	this->registerReplies();
	this->initInfosPerCommand();
	this->motdDir = std::string("./ft_irc.motd");
	this->serverName = std::string("localhost.") + this->port;
	this->startTime = std::time(NULL);
}

Server::~Server(void)
{}

void					Server::initInfosPerCommand(void)
{
	std::map<std::string, int (Server::*)(const Message &, Client *)>::iterator it;

	for (it = this->commands.begin(); it != this->commands.end(); ++it)
		this->infosPerCommand.insert(std::pair<std::string, Info>(it->first, Info()));
}

void					Server::renewFd(const int fd)
{
	FD_SET(fd, &this->readFds);
	if (this->maxFd < fd)
		this->maxFd = fd;
}

void					Server::init(const char *port)
{
	int 			retSocket;
	int				flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	struct addrinfo	*addrInfoIterator;

	/*
	 * TLS SETTING
	 */
	if (std::string(port) == this->tlsPort) {
		std::cout << "tls setting start..." << port << std::endl;
		char CertFile[] = "ft_irc.pem";
		char KeyFile[] = "ft_irc_key.pem";

		SSL_library_init();
		SSL_load_error_strings();

		this->ctx = InitCTX();
		if (this->ctx == NULL)
			exit(0);
		LoadCertificates(this->ctx, CertFile, KeyFile, true);
	}
	flag = 1;
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
			if (ERROR == (retSocket = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			setsockopt(retSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
			if (ERROR == (bind(retSocket, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen)))
				throw Server::SocketBindFailException();
			if (ERROR == (listen(retSocket, SOMAXCONN)))
				throw Server::SocketListenFailException();
		}
	}
	this->renewFd(retSocket);
	if (std::string(port) != this->tlsPort) {
		this->mainSocket = retSocket;
		this->init(this->tlsPort.c_str());
	}
	else
		this->tlsMainSocket = retSocket;
	freeaddrinfo(addrInfo);
}

void					Server::start(void)
{
	struct timeval timeout;
	std::map<int, Client>::iterator next;
	clientIter it;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	while(run)
	{
		FD_SET(this->mainSocket, &readFds);
		FD_SET(this->tlsMainSocket, &readFds);
		it = this->acceptClients.begin();
		next = this->acceptClients.begin();
		if (!this->acceptClients.empty())
			++next;
		while (it != this->acceptClients.end())
		{
			isDeletedClient = false;
			if (it->second.getStatus() != UNKNOWN && it->second.getWaitPong() && it->second.getLastPing() >= PING_CYCLE)
			{
				it->second.setWaitPong(false);
				sendMessage(Message(""
					, "PING"
					, this->prefix)
					, &it->second);
				it->second.setPingLimit(std::time(NULL));
			}
			if (!it->second.getWaitPong() && it->second.getPingLimit() > PING_LIMIT)
			{
				this->disconnectClient(Message(":" + it->second.getInfo(1)
					, TIMEOUT // timeout
					, it->second.getInfo(1)
					  + " :ERROR: ping timeout"), &it->second);
				if (this->acceptClients.empty())
					break;
				isDeletedClient = true;
				it = next;
				// user를 삭제하는 경우에 다른 서버로 quit이 전송 안됨.
			}
			if (!isDeletedClient)
			{
				FD_SET(it->second.getFd(), &readFds);
				if (next != this->acceptClients.end())
					++next;
				++it;
			}
		}
		if (ERROR == select(this->maxFd + 1, &this->readFds, NULL, NULL, &timeout))
			std::cout << ERROR_SELECT_FAIL << std::endl;
		it = this->acceptClients.begin();
		for (int listenFd = this->mainSocket; listenFd <= this->maxFd;)
		{
			if (FD_ISSET(listenFd, &this->readFds))
			{
				if (listenFd == this->mainSocket || listenFd == this->tlsMainSocket)
					this->connectClient(listenFd);
				else
				{
					if (listenFd == this->sslFd)
						this->isSSL = true;
					else
						this->isSSL = false;
					this->receiveMessage(listenFd);
				}
			}
			if ((!this->acceptClients.empty() && it == acceptClients.end()) || !run)
				break;
			if (listenFd < this->tlsMainSocket)
				++listenFd;
			else if (this->acceptClients.empty())
				break;
			else
			{
				listenFd = it->second.getFd();
				++it;
			}
		}
	}
	SSL_CTX_free(this->ctx);
}

void					Server::connectClient(const int &listenFd)
{
	int					newFd;
	struct sockaddr_in6	remoteAddress;
	socklen_t			addressLen;

	addressLen = sizeof(struct sockaddr_in);
	if (ERROR == (newFd = accept(listenFd, (struct sockaddr*)&remoteAddress, &addressLen)))
		throw Server::AcceptFailException();
	fcntl(newFd, F_SETFL, O_NONBLOCK);
	this->renewFd(newFd);
	this->acceptClients.insert(std::pair<int, Client>(newFd, Client(newFd)));
	if (listenFd == this->tlsMainSocket)
	{
		this->ssl = SSL_new(this->ctx);
		SSL_set_fd(this->ssl, newFd);
		if (SSL_accept(this->ssl) == -1)     /* do SSL-protocol accept */
			ERR_print_errors_fp(stderr);
		this->sslFd = newFd;
	}
	std::cout << "Connect client. fd is " << newFd << std::endl;
}

void					Server::receiveMessage(const int &fd)
{
//	char buf[1024];
//	char reply[1024];
//	int sd, bytes;
//	const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";

	char		buffer;
	int			readResult;
	int			connectionStatus;
	Client 		&sender = this->acceptClients[fd];
	Message		sendMessage;

	readResult = 0;
	connectionStatus = CONNECT;
	while (42) {
		if (isSSL)
			readResult = SSL_read(this->ssl, &buffer, 1); /* get request */
		else
			readResult = recv(sender.getFd(), &buffer, 1, 0);
		if (readResult <= 0)
			break;
		sender.addReceivedMessageStr(buffer);
		if (buffer == '\n') {
			if (isSSL)
				std::cout << "SSL ";
			sendMessage = Message(sender.getReceivedMessageStr());
			std::cout << "Reveive message = " << sendMessage.getTotalMessage();
			if (this->commands.find(sendMessage.getCommand()) != this->commands.end()) {
				sender.incrementQueryData(SENDMSG, 1);
				sender.incrementQueryData(SENDBYTES, sendMessage.getTotalMessage().length());
				connectionStatus = (this->*(this->commands[sendMessage.getCommand()]))(sendMessage, &sender);
			}
			sender.clearReceivedMessageStr();
		}
		if (connectionStatus == DISCONNECT || connectionStatus == TOTALDISCONNECT)
			break;
	}
	if (readResult == 0)
		this->disconnectClient(sendMessage, &sender);
	if (connectionStatus == TOTALDISCONNECT)
		this->clearClient();
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
	this->acceptClients[newFd].setCurrentCommand("PASS");
	this->sendMessage(passMessage, &this->acceptClients[newFd]);
	this->acceptClients[newFd].setCurrentCommand("SERVER");
	this->sendMessage(serverMessage, &this->acceptClients[newFd]);
	rOtherServerHandler(Message(), &newClient);
	std::cout << "Connect other server." << std::endl;
}

void					Server::clearClient(void)
{
	for (int i = this->mainSocket; i < this->maxFd; ++i)
	{
		close(i);
		FD_CLR(i, &this->readFds);
	}
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

std::string				Server::getParentServer(std::string key)
{
	if (!this->sendClients.count(key)
		|| this->serverList.count(key)
		|| this->serverName == key)
		return (key);
	return (this->getParentServer(this->sendClients[key].getInfo(UPLINKSERVER)));
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

	stringKey = client->getInfo(1);
	// stringKey가 empty가 아니어야만 client는 정상적인 정보를 가진 상태이다.₩:
	if (!stringKey.empty()) {
		std::vector<std::string> *channelList = client->getSubscribedChannelList();
		if (channelList != NULL) {
			for (size_t i = 0; i < channelList->size(); ++i) {
				if (this->localChannelList.count((*channelList)[i])) {
					this->localChannelList[(*channelList)[i]].leaveUser(client);
					if (this->localChannelList[(*channelList)[i]].getNumbersOfUsers() == 0)
						this->localChannelList.erase((*channelList)[i]);
				}
				if (this->remoteChannelList.count((*channelList)[i])) {
					this->remoteChannelList[(*channelList)[i]].leaveUser(client);
					if (this->remoteChannelList[(*channelList)[i]].getNumbersOfUsers() == 0)
						this->remoteChannelList.erase((*channelList)[i]);
				}
			}
		}
		delete channelList;
	}
	if (this->acceptClients.count(client->getFd()))
	{
		close(client->getFd()); // 다른 메시지가 먼저 오기전에 미리 소켓을 끊어야한다.
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
		if (isSSL) {
			SSL_free(this->ssl);
			this->sslFd = INT_MAX;
		}
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
		incrementLocalByte(client, message);
	else
		incrementRemoteByte(client, message);
	client->incrementQueryData(RECVMSG, 1);
	client->incrementQueryData(RECVBYTES, message.getTotalMessage().length());
	if (client->getFd() == this->sslFd)
	{
		if (ERROR == SSL_write(this->ssl, message.getTotalMessage().c_str(), message.getTotalMessage().length()))
			std::cerr << ERROR_SEND_FAIL << std::endl;
	}
	else
	{
		if (ERROR == send(client->getFd(), message.getTotalMessage().c_str(), message.getTotalMessage().length(), 0))
			std::cerr << ERROR_SEND_FAIL << std::endl;
	}
}

void					Server::broadcastMessage(const Message &message, Client *client)
{
	std::map<std::string, Client*>::iterator	iterator;

	for (iterator = this->serverList.begin(); iterator != this->serverList.end(); ++iterator)
	{
		if (client == NULL || iterator->second->getInfo(SERVERNAME) != client->getInfo(SERVERNAME))
			this->sendMessage(message, iterator->second);
	}
}

// 디버깅
int		Server::show(const Message &message, Client *client)
{
	(void)message;
	(void)client;
	std::cout << "----- clientList ----" << std::endl;
	strClientPtrIter iter = this->clientList.begin();
	std::cout << "( ";
	for (; iter != this->clientList.end(); ++iter)
		std::cout << iter->first << " ";
	std::cout << ")" << std::endl;
	std::cout << "\033[0;32m== server (" << this->serverName << ") channelList ==\033[0m" << std::endl;

	// 이 서버에 있는 채널들의 유저들 출력
	std::map<std::string, Channel>::iterator	it = this->localChannelList.begin();
	for (; it != this->localChannelList.end(); ++it)
	{
		std::cout << "L " << it->first << " ";
		it->second.showUsersName();
	}

	it = this->remoteChannelList.begin();
	for (; it != this->remoteChannelList.end(); it++)
	{
		std::cout << "R " << it->first << " ";
		it->second.showUsersName();
	}
	// 메시지를 보낸 유저의 채널 목록들 출력
	std::cout << "\033[0;32m-- user (" << client->getInfo(NICK) << ") subscribedChannels --\033[0m" << std::endl;
	client->showChannel();
	std::cout << "======================================================================================" << std::endl;

	std::cout << "\033[0;32m-- user (" << client->getInfo(NICK) << ") sendClient --\033[0m" << std::endl;
	for (std::map<std::string, Client>::iterator it = this->sendClients.begin(); it != this->sendClients.end(); ++it)
	{
		std::cout << it->first << " ";
	}
	std::cout << std::endl;
	std::cout << "\033[0;32m-- user (" << client->getInfo(NICK) << ") serverList --\033[0m" << std::endl;
	for (std::map<std::string, Client*>::iterator it = this->serverList.begin(); it != this->serverList.end(); ++it)
	{
		std::cout << it->first << " ";
	}
	std::cout << std::endl;
	std::cout << "======================================================================================" << std::endl;
	return (CONNECT);
}
