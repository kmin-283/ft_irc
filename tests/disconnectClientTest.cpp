#include "Test.hpp"

TEST_GROUP(DisconnectConnectionTest)
{
	Message		sendMessage;
	Client		*client;
	Server		*server;

	void		expect(Message message, Client *clientPtr, Server *serverPtr)
	{
		sendMessage = message;
		client = clientPtr;
		server = serverPtr;
	}
	void		given(unsigned long accpetSize, unsigned long sendSize, unsigned long clientSize, unsigned long serverSize)
	{
		server->disconnectClient(sendMessage, client);
		CHECK_EQUAL(server->acceptClients.size(), accpetSize);
		CHECK_EQUAL(server->sendClients.size(), sendSize);
		CHECK_EQUAL(server->clientList.size(), clientSize);
		CHECK_EQUAL(server->serverList.size(), serverSize);
	}
};

TEST(DisconnectConnectionTest, LocalUserQuit)
{
	int		i;
	int		fd[6];
	char	*result;
	Server	*server;
	Client	*serverOne;
	Client	*serverTwo;
	Client	*localUser;
	Message	sendMessage;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		server = new Server("111", "3000");
		server->serverName = std::string("lo1");

		serverOne = new Client(fd[1], true);
		serverOne->setStatus(SERVER);
		serverOne->setInfo(UPLINKSERVER, std::string("lo1"));
		serverOne->setInfo(SERVERNAME, std::string("lo2"));
		serverOne->setInfo(HOPCOUNT, std::string("1"));
		serverOne->setInfo(SERVERINFO, std::string("hot server"));
		server->acceptClients[serverOne->getFd()] = *serverOne;
		server->sendClients[serverOne->getInfo(SERVERNAME)] = *serverOne;
		server->serverList[serverOne->getInfo(SERVERNAME)] = &server->sendClients[serverOne->getInfo(SERVERNAME)];

		serverTwo = new Client(fd[3], true);
		serverTwo->setStatus(SERVER);
		serverTwo->setInfo(UPLINKSERVER, std::string("lo1"));
		serverTwo->setInfo(SERVERNAME, std::string("lo3"));
		serverTwo->setInfo(HOPCOUNT, std::string("1"));
		serverTwo->setInfo(SERVERINFO, std::string("hot server"));
		server->acceptClients[serverTwo->getFd()] = *serverTwo;
		server->sendClients[serverTwo->getInfo(SERVERNAME)] = *serverTwo;
		server->serverList[serverTwo->getInfo(SERVERNAME)] = &server->sendClients[serverTwo->getInfo(SERVERNAME)];

		localUser = new Client(fd[5], true);
		localUser->setStatus(USER);
		localUser->setInfo(HOSTNAME, std::string("lo1"));
		localUser->setInfo(NICK, std::string("dakim"));
		localUser->setInfo(HOPCOUNT, std::string("1"));
		localUser->setInfo(ADDRESS, std::string("127.0.0.1"));
		localUser->setInfo(USERNAME, std::string("daeok"));
		localUser->setInfo(REALNAME, std::string("deo"));
		server->acceptClients[localUser->getFd()] = *localUser;
		server->sendClients[localUser->getInfo(NICK)] = *localUser;
		server->clientList[localUser->getInfo(NICK)] = &server->sendClients[localUser->getInfo(NICK)];

		CHECK_EQUAL(server->acceptClients.size(), 3);
		CHECK_EQUAL(server->sendClients.size(), 3);
		CHECK_EQUAL(server->clientList.size(), 1);
		CHECK_EQUAL(server->serverList.size(), 2);
		expect(sendMessage, localUser, server);
		given(2, 2, 0, 2);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :Client closed connection\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :Client closed connection\r"));
		free(result);
		i = -1;
		while (++i < 6)
			close(fd[i]);
		delete server;
		delete serverOne;
		delete serverTwo;
		delete localUser;
	}
}

TEST(DisconnectConnectionTest, LocalServerQuit)
{
	int		i;
	int		fd[6];
	char	*result;
	Server	*server;
	Client	*serverOne;
	Client	*serverTwo;
	Message	sendMessage;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
	{
		server = new Server("111", "3000");
		server->serverName = std::string("lo1");

		serverOne = new Client(fd[1], true);
		serverOne->setStatus(SERVER);
		serverOne->setInfo(UPLINKSERVER, std::string("lo1"));
		serverOne->setInfo(SERVERNAME, std::string("lo2"));
		serverOne->setInfo(HOPCOUNT, std::string("1"));
		serverOne->setInfo(SERVERINFO, std::string("hot server"));
		server->acceptClients[serverOne->getFd()] = *serverOne;
		server->sendClients[serverOne->getInfo(SERVERNAME)] = *serverOne;
		server->serverList[serverOne->getInfo(SERVERNAME)] = &server->sendClients[serverOne->getInfo(SERVERNAME)];

		serverTwo = new Client(fd[3], true);
		serverTwo->setStatus(SERVER);
		serverTwo->setInfo(UPLINKSERVER, std::string("lo1"));
		serverTwo->setInfo(SERVERNAME, std::string("lo3"));
		serverTwo->setInfo(HOPCOUNT, std::string("1"));
		serverTwo->setInfo(SERVERINFO, std::string("hot server"));
		server->acceptClients[serverTwo->getFd()] = *serverTwo;
		server->sendClients[serverTwo->getInfo(SERVERNAME)] = *serverTwo;
		server->serverList[serverTwo->getInfo(SERVERNAME)] = &server->sendClients[serverTwo->getInfo(SERVERNAME)];

		CHECK_EQUAL(server->acceptClients.size(), 2);
		CHECK_EQUAL(server->sendClients.size(), 2);
		CHECK_EQUAL(server->clientList.size(), 0);
		CHECK_EQUAL(server->serverList.size(), 2);
		expect(sendMessage, serverOne, server);
		given(1, 1, 0, 1);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo2 SQUIT lo2 :Client closed connection\r"));
		free(result);
		i = -1;
		while (++i < 4)
			close(fd[i]);
		delete server;
		delete serverOne;
		delete serverTwo;
	}
}

TEST(DisconnectConnectionTest, LocalServerHasRemoteServer)
{
	int		i;
	int		fd[6];
	char	*result;
	Server	*server;
	Client	*serverOne;
	Client	*serverTwo;
	Client	*serverThree;
	Client	*serverFour;
	Client	*serverFive;
	Client	*userOne;
	Client	*userTwo;
	Client	*userThree;
	Client	*userFour;
	Message	sendMessage;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
	{
		server = new Server("111", "3000");
		server->serverName = std::string("lo1");

		serverOne = new Client(fd[1], true);
		serverOne->setStatus(SERVER);
		serverOne->setInfo(UPLINKSERVER, std::string("lo1"));
		serverOne->setInfo(SERVERNAME, std::string("lo2"));
		serverOne->setInfo(HOPCOUNT, std::string("1"));
		serverOne->setInfo(SERVERINFO, std::string("hot server"));
		server->acceptClients[serverOne->getFd()] = *serverOne;
		server->sendClients[serverOne->getInfo(SERVERNAME)] = *serverOne;
		server->serverList[serverOne->getInfo(SERVERNAME)] = &server->sendClients[serverOne->getInfo(SERVERNAME)];

		userOne = new Client(fd[1], true);
		userOne->setStatus(USER);
		userOne->setInfo(HOSTNAME, std::string("lo2"));
		userOne->setInfo(NICK, std::string("dakim1"));
		userOne->setInfo(HOPCOUNT, std::string("1"));
		userOne->setInfo(ADDRESS, std::string("127.0.0.1"));
		userOne->setInfo(USERNAME, std::string("daeok"));
		userOne->setInfo(REALNAME, std::string("deo"));
		server->sendClients[userOne->getInfo(NICK)] = *userOne;

		serverTwo = new Client(fd[3], true);
		serverTwo->setStatus(SERVER);
		serverTwo->setInfo(UPLINKSERVER, std::string("lo1"));
		serverTwo->setInfo(SERVERNAME, std::string("lo3"));
		serverTwo->setInfo(HOPCOUNT, std::string("1"));
		serverTwo->setInfo(SERVERINFO, std::string("hot server"));
		server->acceptClients[serverTwo->getFd()] = *serverTwo;
		server->sendClients[serverTwo->getInfo(SERVERNAME)] = *serverTwo;
		server->serverList[serverTwo->getInfo(SERVERNAME)] = &server->sendClients[serverTwo->getInfo(SERVERNAME)];

		serverThree = new Client(fd[1], true);
		serverThree->setStatus(SERVER);
		serverThree->setInfo(UPLINKSERVER, std::string("lo2"));
		serverThree->setInfo(SERVERNAME, std::string("lo4"));
		serverThree->setInfo(HOPCOUNT, std::string("2"));
		serverThree->setInfo(SERVERINFO, std::string("hot server"));
		server->sendClients[serverThree->getInfo(SERVERNAME)] = *serverThree;

		userTwo = new Client(fd[1], true);
		userTwo->setStatus(USER);
		userTwo->setInfo(HOSTNAME, std::string("lo4"));
		userTwo->setInfo(NICK, std::string("dakim2"));
		userTwo->setInfo(HOPCOUNT, std::string("1"));
		userTwo->setInfo(ADDRESS, std::string("127.0.0.1"));
		userTwo->setInfo(USERNAME, std::string("daeok"));
		userTwo->setInfo(REALNAME, std::string("deo"));
		server->sendClients[userTwo->getInfo(NICK)] = *userTwo;

		serverFour = new Client(fd[1], true);
		serverFour->setStatus(SERVER);
		serverFour->setInfo(UPLINKSERVER, std::string("lo4"));
		serverFour->setInfo(SERVERNAME, std::string("lo5"));
		serverFour->setInfo(HOPCOUNT, std::string("2"));
		serverFour->setInfo(SERVERINFO, std::string("hot server"));
		server->sendClients[serverFour->getInfo(SERVERNAME)] = *serverFour;

		userThree = new Client(fd[1], true);
		userThree->setStatus(USER);
		userThree->setInfo(HOSTNAME, std::string("lo5"));
		userThree->setInfo(NICK, std::string("dakim3"));
		userThree->setInfo(HOPCOUNT, std::string("1"));
		userThree->setInfo(ADDRESS, std::string("127.0.0.1"));
		userThree->setInfo(USERNAME, std::string("daeok"));
		userThree->setInfo(REALNAME, std::string("deo"));
		server->sendClients[userThree->getInfo(NICK)] = *userThree;

		serverFive = new Client(fd[1], true);
		serverFive->setStatus(SERVER);
		serverFive->setInfo(UPLINKSERVER, std::string("lo5"));
		serverFive->setInfo(SERVERNAME, std::string("lo6"));
		serverFive->setInfo(HOPCOUNT, std::string("3"));
		serverFive->setInfo(SERVERINFO, std::string("hot server"));
		server->sendClients[serverFive->getInfo(SERVERNAME)] = *serverFive;

		userFour = new Client(fd[1], true);
		userFour->setStatus(USER);
		userFour->setInfo(HOSTNAME, std::string("lo6"));
		userFour->setInfo(NICK, std::string("dakim4"));
		userFour->setInfo(HOPCOUNT, std::string("1"));
		userFour->setInfo(ADDRESS, std::string("127.0.0.1"));
		userFour->setInfo(USERNAME, std::string("daeok"));
		userFour->setInfo(REALNAME, std::string("deo"));
		server->sendClients[userFour->getInfo(NICK)] = *userFour;

		CHECK_EQUAL(server->acceptClients.size(), 2);
		CHECK_EQUAL(server->sendClients.size(), 9);
		CHECK_EQUAL(server->clientList.size(), 0);
		CHECK_EQUAL(server->serverList.size(), 2);
		expect(sendMessage, serverOne, server);
		given(1, 1, 0, 1);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo6 SQUIT lo6 :lo1 lo2\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo5 SQUIT lo5 :lo1 lo2\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo4 SQUIT lo4 :lo1 lo2\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo2 SQUIT lo2 :Client closed connection\r"));
		free(result);
		i = -1;
		while (++i < 4)
			close(fd[i]);
		delete server;
		delete serverOne;
		delete serverTwo;
		delete serverThree;
		delete serverFour;
		delete serverFive;
		delete userOne;
		delete userTwo;
		delete userThree;
		delete userFour;
	}
}
