#include "Test.hpp"

TEST_GROUP(LocalServerErrorTest)
{
	int			fd[2];
	char		*result;
	Client		*client;
	Message		message;
	int			connectionStatus;

	void		expect(Message sendMessage, bool authority)
	{
		client = NULL;
		if (pipe(fd) != -1)
		{
			client = new Client(fd[1], authority);
			client->setStatus(UNKNOWN);
			message = sendMessage;
		}
	}
	void		given(Server server, int expectStatus, std::string expectStr)
	{
		if (client != NULL)
		{
			connectionStatus = server.serverHandler(message, client);
			get_next_line(fd[0], &result);
			CHECK_EQUAL(connectionStatus, expectStatus);
			CHECK_EQUAL(std::string(result), expectStr);
			free(result);
			delete client;
		}
	}
};

TEST(LocalServerErrorTest, NotAuthorizedSyntaxError)
{
	Message		message;
	Server		server("111", "3000");

	message = Message("SERVER\r\n");
	expect(message, false);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost.123\r\n");
	expect(message, false);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost.123 1\r\n");
	expect(message, false);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost.123 sdfsdf :1\r\n");
	expect(message, false);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost123 1 :1\r\n");
	expect(message, false);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
}

TEST(LocalServerErrorTest, AuthorizedSyntaxError)
{
	Message		message;
	Server		server("111", "3000");

	message = Message("SERVER\r\n");
	expect(message, true);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost.123\r\n");
	expect(message, true);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost.123 1\r\n");
	expect(message, true);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost123 1 :1\r\n");
	expect(message, true);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
	message = Message("SERVER localhost123 1 0 :1\r\n");
	expect(message, true);
	given(server, CONNECT, std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
}

TEST(LocalServerErrorTest, ServerNameOverlap)
{
	Message		message;
	Server		server("111", "3000");
	Client		otherServer;

	message = Message("SERVER localhost.3000 1 :1\r\n");
	expect(message, true);
	given(server, DISCONNECT, std::string(":localhost.3000 462 :ID localhost.3000 already registered\r"));
	otherServer.setInfo(SERVERNAME, "localhost.123");
	server.sendClients[otherServer.getInfo(SERVERNAME)] = otherServer;
	message = Message("SERVER localhost.123 1 :1\r\n");
	expect(message, true);
	given(server, DISCONNECT, std::string(":localhost.3000 462 :ID localhost.123 already registered\r"));
}

TEST_GROUP(LocalServerTest)
{
	Server		*server;
	Client		*client;

	void		expect(Server *serverPtr, Client *clientPtr)
	{
		client = clientPtr;
		server = serverPtr;
	}
	void		given(std::string upLinkServer, std::string serverName,
				std::string hopCount, std::string serverInfo, ClientStatus status)
	{
		if (client != NULL)
		{
			CHECK_EQUAL(client->getStatus(), status);
			CHECK_EQUAL(client->getInfo(UPLINKSERVER), upLinkServer);
			CHECK_EQUAL(client->getInfo(SERVERNAME), serverName);
			CHECK_EQUAL(client->getInfo(HOPCOUNT), hopCount);
			CHECK_EQUAL(client->getInfo(SERVERINFO), serverInfo);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(UPLINKSERVER), upLinkServer);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(SERVERNAME), serverName);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(HOPCOUNT), hopCount);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(SERVERINFO), serverInfo);
			CHECK_EQUAL(server->serverList.count(serverName), 1);
			CHECK_EQUAL(server->serverList[serverName]->getInfo(UPLINKSERVER), upLinkServer);
			CHECK_EQUAL(server->serverList[serverName]->getInfo(SERVERNAME), serverName);
			CHECK_EQUAL(server->serverList[serverName]->getInfo(HOPCOUNT), hopCount);
			CHECK_EQUAL(server->serverList[serverName]->getInfo(SERVERINFO), serverInfo);
		}
	}
};

TEST(LocalServerTest, RegisterServer)
{
	int			fd[2];
	char		*result;
	Client		*client;
	std::string	expectStr;
	Message		serverMessage;
	Server		server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		serverMessage = Message("SERVER localhost.3001 1 :kikik kiki\r\n");
		expect(&server, client);
		server.serverHandler(serverMessage, client);
		given(std::string("localhost.3000"), std::string("localhost.3001"), std::string("1"), std::string(":kikik kiki"), SERVER);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
		free(result);
		close(fd[1]);
		close(fd[0]);
		delete client;
	}
}

TEST(LocalServerTest, RegisterServerBroadCastUserOne)
{
	int			fd[2];
	char		*result;
	Client		*client;
	Client		*localUser;
	std::string	expectStr;
	Message		serverMessage;
	Server		server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		localUser = new Client(0, true);
		localUser->setStatus(USER);
		localUser->setInfo(HOSTNAME, std::string("localhost.3000"));
		localUser->setInfo(NICK, std::string("dakim"));
		localUser->setInfo(HOPCOUNT, std::string("1"));
		localUser->setInfo(ADDRESS, std::string("127.0.0.1"));
		localUser->setInfo(USERNAME, std::string("deok"));
		localUser->setInfo(REALNAME, std::string("dek"));
		server.sendClients[std::string("dakim")] = *localUser;
		server.clientList[std::string("dakim")] = &server.sendClients[std::string("dakim")];
		server.prefix = std::string(":localhost.3000");
		server.serverName = std::string("localhost.3000");
		server.info = std::string(": kmin seunkim dakim made this server.");
		serverMessage = Message("SERVER localhost.3001 1 :kikik kiki\r\n");
		expect(&server, client);
		server.serverHandler(serverMessage, client);
		given(std::string("localhost.3000"), std::string("localhost.3001"), std::string("1"), std::string(":kikik kiki"), SERVER);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~deok 127.0.0.1 localhost.3000 :dek\r"));
		free(result);
		close(fd[1]);
		close(fd[0]);
		delete client;
		delete localUser;
	}
}

TEST(LocalServerTest, RegisterServerHopCountNotMatch)
{
	int			fd[2];
	char		*result;
	Client		*client;
	std::string	expectStr;
	Message		serverMessage;
	Server		server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		serverMessage = Message("SERVER localhost.3001 1000 :kikik kiki\r\n");
		expect(&server, client);
		server.serverHandler(serverMessage, client);
		given(std::string("localhost.3000"), std::string("localhost.3001"), std::string("1"), std::string(":kikik kiki"), SERVER);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
		free(result);
		close(fd[1]);
		close(fd[0]);
		delete client;
	}
}

TEST(LocalServerTest, RegisterServerBroadcast)
{
	int			i;
	int			fd[6];
	char		*result;
	Client		*client;
	Client		*otherServer;
	Client		*anotherServer;
	std::string	expectStr;
	Message		serverMessage;
	Server		server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		i = -1;
		client = new Client(fd[1], true);
		otherServer = new Client(fd[3], true);
		anotherServer = new Client(fd[5], true);
		otherServer->setInfo(SERVERNAME, std::string("oherServer"));
		otherServer->setStatus(SERVER);
		anotherServer->setInfo(SERVERNAME, std::string("anotherServer"));
		anotherServer->setStatus(SERVER);
		serverMessage = Message("SERVER localhost.3001 1 :kikik kiki\r\n");
		server.serverList["otherServer"] = otherServer;
		server.serverList["anotherServer"] = anotherServer;
		expect(&server, client);
		server.serverHandler(serverMessage, client);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3001 2 0 :kikik kiki\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3001 2 0 :kikik kiki\r"));
		free(result);
		while (++i < 6)
			close(fd[i]);
		delete client;
		delete otherServer;
		delete anotherServer;
	}
}

TEST(LocalServerTest, RegisterServerBroadCastUserTwo)
 {
 	int			fd[2];
 	char		*result;
 	Client		*client;
 	Client		*localUser;
 	std::string	expectStr;
 	Message		serverMessage;
 	Server		server("111", "3000");

 	if (pipe(fd) != -1)
 	{
 		client = new Client(fd[1], true);
 		localUser = new Client(0, true);
 		localUser->setStatus(USER);
 		localUser->setInfo(HOSTNAME, std::string("localhost.3000"));
 		localUser->setInfo(NICK, std::string("dakim"));
 		localUser->setInfo(HOPCOUNT, std::string("1"));
 		localUser->setInfo(ADDRESS, std::string("127.0.0.1"));
 		localUser->setInfo(USERNAME, std::string("deok"));
 		localUser->setInfo(REALNAME, std::string("dek"));
 		server.sendClients[std::string("dakim")] = *localUser;
 		server.clientList[std::string("dakim")] = &server.sendClients[std::string("dakim")];
 		server.prefix = std::string(":localhost.3000");
 		server.serverName = std::string("localhost.3000");
 		server.info = std::string(": kmin seunkim dakim made this server.");
 		serverMessage = Message("SERVER localhost.3001 1 :kikik kiki\r\n");
 		expect(&server, client);
 		server.serverHandler(serverMessage, client);
 		given(std::string("localhost.3000"), std::string("localhost.3001"), std::string("1"), std::string(":kikik kiki"), SERVER);
 		get_next_line(fd[0], &result);
 		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
 		free(result);
 		get_next_line(fd[0], &result);
 		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
 		free(result);
 		get_next_line(fd[0], &result);
 		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
 		free(result);
 		get_next_line(fd[0], &result);
 		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~deok 127.0.0.1 localhost.3000 :dek\r"));
 		free(result);
 		close(fd[1]);
 		close(fd[0]);
 		delete client;
 		delete localUser;
 	}
 }



TEST_GROUP(RemoteServerErrorTest)
{
	char		*result;
	Client		*client;
	int			connectionStatus;
	Message		remoteServerMessage;
	Message		localServerMessage;

	void		expect(Message sendMessage, Client *clientPtr)
	{
		client = clientPtr;
		client->setStatus(UNKNOWN);
		remoteServerMessage = sendMessage;
	}
	void		given(Server server, int expectStatus, int fd, unsigned long serverCount)
	{
		localServerMessage = Message("SERVER localhost.3001 1 :1\r\n");
		connectionStatus = server.serverHandler(localServerMessage, client);
		get_next_line(fd, &result);
		CHECK_EQUAL(connectionStatus, CONNECT);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
		free(result);
		get_next_line(fd, &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
		free(result);
		connectionStatus = server.serverHandler(remoteServerMessage, client);
		CHECK_EQUAL(connectionStatus, expectStatus);
		if (!remoteServerMessage.getParameters().empty())
		{
			CHECK_EQUAL(server.sendClients.count(remoteServerMessage.getParameter(0)), serverCount);
		}
	}
};

TEST(RemoteServerErrorTest, PrefixError)
{
	int			fd[2];
	Message		message;
	Client		*client;
	Server		server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		message = Message("SERVER localhost.3002 2 0 :123\r\n");
		expect(message, client);
		given(server, CONNECT, fd[0], 0);
		delete client;
		// client = new Client(fd[1], true);
		// message = Message("localhost.3001 SERVER localhost.3002 2 0 :123\r\n");
		// expect(message, client);
		// given(server, CONNECT, fd[0], 0);
		// delete client;
		client = new Client(fd[1], true);
		message = Message(":localhost.123123 SERVER localhost.3002 2 0 :123\r\n");
		expect(message, client);
		given(server, CONNECT, fd[0], 0);
		delete client;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(RemoteServerErrorTest, ParameterError)
{
	int			fd[2];
	Message		message;
	char		*result;
	Client		*client;
	Server		server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		message = Message(":localhost.3001 SERVER\r\n");
		expect(message, client);
		given(server, CONNECT, fd[0], 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 localhost.3001 SERVER :Syntax error\r"));
		free(result);
		delete client;
		client = new Client(fd[1], true);
		message = Message(":localhost.3001 SERVER localhost.3002\r\n");
		expect(message, client);
		given(server, CONNECT, fd[0], 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 localhost.3001 SERVER :Syntax error\r"));
		free(result);
		delete client;
		client = new Client(fd[1], true);
		message = Message(":localhost.3001 SERVER localhost.3002 1\r\n");
		expect(message, client);
		given(server, CONNECT, fd[0], 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 localhost.3001 SERVER :Syntax error\r"));
		free(result);
		delete client;
		client = new Client(fd[1], true);
		message = Message(":localhost.3001 SERVER localhost.3002 1 :1\r\n");
		expect(message, client);
		given(server, CONNECT, fd[0], 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 localhost.3001 SERVER :Syntax error\r"));
		free(result);
		delete client;
		client = new Client(fd[1], true);
		message = Message(":localhost.3001 SERVER localhost.3002 1 1\r\n");
		expect(message, client);
		given(server, CONNECT, fd[0], 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 localhost.3001 SERVER :Syntax error\r"));
		free(result);
		delete client;
		close(fd[0]);
		close(fd[1]);
	}
}

// TODO serverHandler 리턴값
// TODO 메시지 전송 서버가 받는 메시지
// TODO 다른 서버들 squit메시지
TEST(RemoteServerErrorTest, ServerNameError)
{
	int			i;
	int			fd[6];
	Message		clientMessage;
	Message		otherServerMessage;
	Message		anotherServerMessage;
	char		*result;
	Client		*client;
	Client		*otherServer;
	Client		*anotherServer;
	Server		*server;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		server = new Server("111", "3000");
		client = new Client(fd[1], true);
		otherServer = new Client(fd[3], true);
		anotherServer = new Client(fd[5], true);
		clientMessage = Message(":localhost.3001 SERVER localhost.3001 1 0 :1\r\n");
		otherServer->setInfo(SERVERNAME, std::string("localhost.3002"));
		otherServer->setStatus(SERVER);
		anotherServer->setInfo(SERVERNAME, std::string("localhost.3003"));
		anotherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("localhost.3000"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string(":1"));
		anotherServer->setInfo(UPLINKSERVER, std::string("localhost.3000"));
		anotherServer->setInfo(HOPCOUNT, std::string("1"));
		anotherServer->setInfo(SERVERINFO, std::string(":1"));
		server->serverList["localhost.3002"] = otherServer;
		server->serverList["localhost.3003"] = anotherServer;
		server->sendClients["localhost.3002"] = *otherServer;
		server->sendClients["localhost.3003"] = *anotherServer;
		expect(clientMessage, client);
		given(*server, DISCONNECT, fd[0], 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3002 2 0 :1\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3003 2 0 :1\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 462 :ID localhost.3001 already registered\r"));
		free(result);

		// get_next_line(fd[2], &result);
		// CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SQUIT localhost.3001 :ID \"localhost.3001\" already registered\r"));
		// free(result);

		// get_next_line(fd[4], &result);
		// CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SQUIT localhost.3001 :ID \"localhost.3001\" already registered\r"));
		// free(result);

		delete server;
		delete client;
		delete otherServer;
		delete anotherServer;

		server = new Server("111", "3000");
		client = new Client(fd[1], true);
		otherServer = new Client(fd[3], true);
		anotherServer = new Client(fd[5], true);
		otherServerMessage = Message("SERVER localhost.3002 1 :1\r\n");
		anotherServerMessage = Message("SERVER localhost.3003 1 :1\r\n");
		clientMessage = Message(":localhost.3001 SERVER localhost.3000 1 0 :1\r\n");
		otherServer->setInfo(SERVERNAME, std::string("localhost.3002"));
		otherServer->setStatus(SERVER);
		anotherServer->setInfo(SERVERNAME, std::string("localhost.3003"));
		anotherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("localhost.3000"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string(":1"));
		anotherServer->setInfo(UPLINKSERVER, std::string("localhost.3000"));
		anotherServer->setInfo(HOPCOUNT, std::string("1"));
		anotherServer->setInfo(SERVERINFO, std::string(":1"));
		server->serverList["localhost.3002"] = otherServer;
		server->serverList["localhost.3003"] = anotherServer;
		server->sendClients["localhost.3002"] = *otherServer;
		server->sendClients["localhost.3003"] = *anotherServer;
		expect(clientMessage, client);
		given(*server, DISCONNECT, fd[0], 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3002 2 0 :1\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3003 2 0 :1\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 462 :ID localhost.3000 already registered\r"));
		free(result);
		// get_next_line(fd[2], &result);
		// CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SQUIT localhost.3001 :ID \"localhost.3001\" already registered\r"));
		// free(result);
		// get_next_line(fd[4], &result);
		// CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SQUIT localhost.3001 :ID \"localhost.3001\" already registered\r"));
		// free(result);

		delete server;
		delete client;
		delete otherServer;
		delete anotherServer;

		server = new Server("111", "3000");
		client = new Client(fd[1], true);
		otherServer = new Client(fd[3], true);
		anotherServer = new Client(fd[5], true);
		otherServerMessage = Message("SERVER localhost.3002 1 :1\r\n");
		anotherServerMessage = Message("SERVER localhost.3003 1 :1\r\n");
		clientMessage = Message(":localhost.3001 SERVER localhost.3002 1 0 :1\r\n");
		otherServer->setInfo(SERVERNAME, std::string("localhost.3002"));
		otherServer->setStatus(SERVER);
		anotherServer->setInfo(SERVERNAME, std::string("localhost.3003"));
		anotherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("localhost.3000"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string(":1"));
		anotherServer->setInfo(UPLINKSERVER, std::string("localhost.3000"));
		anotherServer->setInfo(HOPCOUNT, std::string("1"));
		anotherServer->setInfo(SERVERINFO, std::string(":1"));
		server->serverList["localhost.3002"] = otherServer;
		server->serverList["localhost.3003"] = anotherServer;
		server->sendClients["localhost.3002"] = *otherServer;
		server->sendClients["localhost.3003"] = *anotherServer;
		expect(clientMessage, client);
		given(*server, DISCONNECT, fd[0], 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3002 2 0 :1\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3003 2 0 :1\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 462 :ID localhost.3002 already registered\r"));
		free(result);
		// get_next_line(fd[2], &result);
		// CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SQUIT localhost.3001 :ID \"localhost.3001\" already registered\r"));
		// free(result);
		// get_next_line(fd[4], &result);
		// CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SQUIT localhost.3001 :ID \"localhost.3001\" already registered\r"));
		// free(result);

		delete server;
		delete client;
		delete otherServer;
		delete anotherServer;

		i = -1;
		while (++i < 6)
			close(fd[i]);
	}
}

// // TODO hopCount, token이 숫자가 아닌경우
// TEST(RemoteServerErrorTest, HopCountTokenError)
// {
// 	int			fd[2];
// 	Message		message;
// 	char		*result;
// 	Client		*client;
// 	Server		server("111", "3000");

// 	if (pipe(fd) != -1)
// 	{
// 		client = new Client(fd[1], true);
// 		message = Message(":localhost.3001 SERVER localhost.3002 sfsdfsff 0 :123\r\n");
// 		expect(message, client);
// 		given(server, CONNECT, fd[0], 0);
// 		get_next_line(fd[0], &result);
// 		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 localhost.3001 SERVER :Syntax error\r"));
// 		delete client;
// 		client = new Client(fd[1], true);
// 		message = Message(":localhost.3001 SERVER localhost.3002 2 fsdffsdf :123\r\n");
// 		expect(message, client);
// 		given(server, CONNECT, fd[0], 0);
// 		get_next_line(fd[0], &result);
// 		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 localhost.3001 SERVER :Syntax error\r"));
// 		delete client;
// 		close(fd[0]);
// 		close(fd[1]);
// 	}
// }

// TODO serverHandler 리턴값
// TODO 메시지 에러
TEST(RemoteServerErrorTest, ServerMessageSendAfterNick)
{
	int		fd[2];
	int		status;
	Server	server("111", "3000");
	Message	nickMessage;
	Message	serverMessage;
	Client	*client;

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1],true);
		nickMessage = Message("NICK dakim\r\n");
		server.nickHandler(nickMessage, client);
		serverMessage = Message("SERVER localhost.3001 1 :1\r\n");
		status = server.serverHandler(serverMessage, client);
		CHECK_EQUAL(CONNECT, status);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 dakim SERVER :Syntax error\r"));
		free(result);
		delete client;
	}
}

TEST(RemoteServerErrorTest, ServerMessageSendAfterUser)
{
	int		fd[2];
	int		status;
	Server	server("111", "3000");
	Message	nickMessage;
	Message	serverMessage;
	Client	*client;

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1],true);
		nickMessage = Message("USER dakim 123 123 :123\r\n");
		server.userHandler(nickMessage, client);
		serverMessage = Message("SERVER localhost.3001 1 :1\r\n");
		status = server.serverHandler(serverMessage, client);
		CHECK_EQUAL(CONNECT, status);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 461 * SERVER :Syntax error\r"));
		free(result);
		delete client;
	}
}

TEST_GROUP(RegisterRemoteServerTest)
{
	Server		*server;
	Client		*client;

	void		expect(Server *serverPtr, Client *clientPtr)
	{
		client = clientPtr;
		server = serverPtr;
	}
	void		given(std::string upLinkServer, std::string serverName,
				std::string hopCount, std::string serverInfo, ClientStatus status)
	{
		if (client != NULL)
		{
			CHECK_EQUAL(client->getStatus(), status);
			CHECK_EQUAL(client->getInfo(UPLINKSERVER), upLinkServer);
			CHECK_EQUAL(client->getInfo(SERVERNAME), serverName);
			CHECK_EQUAL(client->getInfo(HOPCOUNT), hopCount);
			CHECK_EQUAL(client->getInfo(SERVERINFO), serverInfo);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(UPLINKSERVER), upLinkServer);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(SERVERNAME), serverName);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(HOPCOUNT), hopCount);
			CHECK_EQUAL(server->sendClients[serverName].getInfo(SERVERINFO), serverInfo);
		}
	}
};

TEST(RegisterRemoteServerTest, RegisterRemoteServer)
{
	int		i;
	int		fd[6];
	char	*result;
	Client	*client;
	Client	*otherServer;
	Client	*anotherServer;
	Message	otherMessage;
	Message	clientMessage;
	Message	anotherMessage;
	Message remoteMessage;
	Server	server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		client = new Client(fd[1], true);
		otherServer = new Client(fd[3], true);
		anotherServer = new Client(fd[5], true);
		clientMessage = Message("SERVER localhost.3001 1 :1\r\n");
		otherMessage = Message("SERVER localhost.3002 1 :1\r\n");
		anotherMessage = Message("SERVER localhost.3003 1 :1\r\n");
		server.serverHandler(clientMessage, client);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
		free(result);
		server.serverHandler(otherMessage, otherServer);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3002 2 0 :1\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3001 2 0 :1\r"));
		free(result);
		server.serverHandler(anotherMessage, anotherServer);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 PASS 111\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server.\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3003 2 0 :1\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3003 2 0 :1\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3001 2 0 :1\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 SERVER localhost.3002 2 0 :1\r"));
		free(result);
		remoteMessage = Message(":localhost.3001 SERVER localhost.3004 2 0 :1\r\n");
		server.serverHandler(remoteMessage, client);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SERVER localhost.3004 3 0 :1\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3001 SERVER localhost.3004 3 0 :1\r"));
		free(result);
		// get_next_line(fd[0], &result);
		// CHECK_EQUAL(std::string(result), std::string(""));
		CHECK_EQUAL(server.sendClients.size(), 4);
		CHECK_EQUAL(server.serverList.size(), 3);
		i = -1;
		while (++i < 6)
			close(fd[i]);
		delete client;
		delete otherServer;
		delete anotherServer;
	}
}
