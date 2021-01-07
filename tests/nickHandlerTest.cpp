#include "Test.hpp"

TEST_GROUP(NickHandlerErrorTest)
{
	int			fd[2];
	char		*result;
	Client		*client;
	Message		*message;
	std::string	resultStr;
	int			connectStatus;

	void		expect(std::string nick, std::string messageStr, bool authority,
				ClientStatus status, std::string userName, std::string serverName)
	{
		message = NULL;
		if (pipe(fd) != -1)
		{
			message = new Message(messageStr);
			client = new Client(fd[1], authority);
			client->setStatus(status);
			client->setInfo(NICK, nick);
			client->setInfo(USERNAME, userName);
			client->setInfo(HOSTNAME, serverName);
		}
	}
	void		given(Server &server, std::string expectStr, int expectConnection,
				std::string nick, ClientStatus status)
	{
		if (message != NULL && client != NULL)
		{
			connectStatus = server.nickHandler(*message, client);
			CHECK_EQUAL(connectStatus, expectConnection);
			get_next_line(fd[0], &result);
			resultStr = std::string(result) + std::string("\n");
			CHECK_EQUAL(resultStr, expectStr);
			CHECK_EQUAL(client->getInfo(NICK), nick);
			CHECK_EQUAL(client->getStatus(), status);
			close(fd[1]);
			close(fd[0]);
			free(result);
			delete message;
			delete client;
		}
	}
};

TEST(NickHandlerErrorTest, UnknownParameterEmptyTest)
{
	Server		server("111", "3000");

	expect("", "NICK\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "", UNKNOWN);
	expect("", "NICK\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "", UNKNOWN);
	expect("dakim", "NICK\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "dakim", UNKNOWN);
	expect("dakim", "NICK\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "dakim", UNKNOWN);
}

TEST(NickHandlerErrorTest, UnknownTooMuchParameterTest)
{
	Server		server("111", "3000");

	expect("", "NICK da da\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 461 * NICK :Syntax error\r\n", CONNECT, "", UNKNOWN);
	expect("", "NICK da da\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 461 * NICK :Syntax error\r\n", CONNECT, "", UNKNOWN);
	expect("dakim", "NICK da da\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 461 dakim NICK :Syntax error\r\n", CONNECT, "dakim", UNKNOWN);
	expect("dakim", "NICK da da\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 461 dakim NICK :Syntax error\r\n", CONNECT, "dakim", UNKNOWN);
}

TEST(NickHandlerErrorTest, UnknownNickFormErrorTest)
{
	Server		server("111", "3000");

	expect("", "NICK !dakim\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("", "NICK !dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("dakim", "NICK !dakim\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);
	expect("dakim", "NICK !dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);

	expect("", "NICK 1dakim\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("", "NICK 1dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("dakim", "NICK 1dakim\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);
	expect("dakim", "NICK 1dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);

	expect("", "NICK d!akim\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("", "NICK d!akim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("dakim", "NICK d!akim\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);
	expect("dakim", "NICK d!akim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);

	expect("", "NICK dadadadadadada\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("", "NICK dadadadadadada\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "", UNKNOWN);
	expect("dakim", "NICK dadadadadadada\r\n", false, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);
	expect("dakim", "NICK dadadadadadada\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "dakim", UNKNOWN);
}

TEST(NickHandlerErrorTest, PassFail)
{
	Server		server("111", "3000");

	expect("", "NICK dakim\r\n", false, UNKNOWN, "", "");
	given(server, "ERROR :Access denied: Bad password\r\n", DISCONNECT, "", UNKNOWN);
}

TEST(NickHandlerErrorTest, NickOverlap)
{
	Server		server("111", "3000");

	server.prefix = std::string(":lolo");
	server.serverName = std::string("lolo");
	server.sendClients["dakim"] = Client(0, true);
	expect("", "NICK dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":lolo 433 * dakim :Nickname already in use\r\n", CONNECT, "", UNKNOWN);
	expect("da", "NICK dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":lolo 433 da dakim :Nickname already in use\r\n", CONNECT, "da", UNKNOWN);
	expect("da", "NICK lolo\r\n", true, UNKNOWN, "", "");
	given(server, ":lolo 433 da lolo :Nickname already in use\r\n", CONNECT, "da", UNKNOWN);
}

TEST_GROUP(NickHandlerTest)
{
	int			fd[2];
	Client		*client;
	Message		*message;
	int			connectStatus;

	void		expect(std::string nick, std::string messageStr, bool authority, ClientStatus status)
	{
		message = NULL;
		if (pipe(fd) != -1)
		{
			message = new Message(messageStr);
			client = new Client(fd[1], authority);
			client->setStatus(status);
			client->setInfo(NICK, nick);
		}
	}
	void		given(Server &server, int expectConnection,
						std::string nick, ClientStatus status)
	{
		if (message != NULL && client != NULL)
		{
			connectStatus = server.nickHandler(*message, client);
			CHECK_EQUAL(connectStatus, expectConnection);
			CHECK_EQUAL(client->getInfo(NICK), nick);
			CHECK_EQUAL(client->getStatus(), status);
			close(fd[1]);
			close(fd[0]);
			delete message;
			delete client;
		}
	}
};

TEST(NickHandlerTest, RegisterNick)
{
	Server		server("111", "3000");

	expect("", "NICK dakim\r\n", true, UNKNOWN);
	CHECK_EQUAL(server.sendClients.size(), 0);
	CHECK_EQUAL(server.clientList.size(), 0);
	given(server, CONNECT, "dakim", UNKNOWN);
	if (server.sendClients.find("dakim") != server.sendClients.end())
		CHECK_EQUAL(1,1);
	else
		CHECK_EQUAL(1, 0);
	CHECK_EQUAL(server.sendClients.size(), 1);
	CHECK_EQUAL(server.clientList.size(), 1);
	expect("dakim", "NICK dakim1\r\n", true, UNKNOWN);
	given(server, CONNECT, "dakim1", UNKNOWN);
	if (server.sendClients.find("dakim") == server.sendClients.end())
		CHECK_EQUAL(1,1);
	else
		CHECK_EQUAL(1, 0);
	if (server.sendClients.find("dakim1") != server.sendClients.end())
		CHECK_EQUAL(1,1);
	else
		CHECK_EQUAL(1, 0);
	CHECK_EQUAL(server.sendClients["dakim1"].getInfo(NICK), std::string("dakim1"));
	CHECK_EQUAL(server.sendClients.size(), 1);
	CHECK_EQUAL(server.clientList.size(), 1);
}


TEST_GROUP(UserSendNickMessageTest)
{
	int			fd[2];
	char		*result;
	Client		*client;
	std::string	resultStr;
	Message		sendMessage;

	void			expect(Message message)
	{
		client = NULL;
		if (pipe(fd) != -1)
		{
			client = new Client(fd[1], true);
			client->setStatus(USER);
			client->setInfo(HOSTNAME, std::string("lo1"));
			client->setInfo(NICK, std::string("dakim"));
			client->setInfo(ADDRESS, std::string("127.0.0.1"));
			client->setInfo(USERNAME, std::string("dak"));
			client->setInfo(REALNAME, std::string("deok"));
			sendMessage = message;
		}
	}
	void			given(Server &server, int connection, std::string expectStr, std::string nick)
	{
		if (client != NULL)
		{
			server.prefix = std::string(":lo1");
			server.serverName = std::string("lo1");
			server.sendClients["dakim"] = *client;
			server.clientList["dakim"] = &server.sendClients["dakim"];
			CHECK_EQUAL(connection, server.nickHandler(sendMessage, client));
			get_next_line(fd[0], &result);
			CHECK_EQUAL(std::string(result), expectStr);
			free(result);
			CHECK_EQUAL(client->getInfo(HOSTNAME), std::string("lo1"));
			CHECK_EQUAL(client->getInfo(NICK), nick);
			CHECK_EQUAL(client->getInfo(ADDRESS), std::string("127.0.0.1"));
			CHECK_EQUAL(client->getInfo(USERNAME), std::string("dak"));
			CHECK_EQUAL(client->getInfo(REALNAME), std::string("deok"));
			CHECK_EQUAL(server.sendClients.count(nick), 1);
			delete client;
		}
	}
};

TEST(UserSendNickMessageTest, PrefixError)
{
	Server server("111", "3000");

	expect(Message(std::string(":"), std::string("NICK"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"\"\r"), std::string("dakim"));
	expect(Message(std::string(":d"), std::string("NICK"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \":d\"\r"), std::string("dakim"));
	expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"), std::string("dakim"));
	expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"), std::string("dakim"));
	expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"), std::string("dakim"));
	expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("d d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"), std::string("dakim"));
	expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("d d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"), std::string("dakim"));
	expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("d d d d d d d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"), std::string("dakim"));
}

TEST(UserSendNickMessageTest, ParameterError)
{
	Server server("111", "3000");

	expect(Message(std::string(""), std::string("NICK"), std::string("")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("d d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("d d d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("d d d d d d d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));

	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("d d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("d d d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("d d d d d d d d")));
	given(server, CONNECT, std::string(":lo1 461 dakim NICK :Syntax error\r"), std::string("dakim"));
}

TEST(UserSendNickMessageTest, NotVaildNick)
{
	Server server("111", "3000");

	expect(Message(std::string(""), std::string("NICK"), std::string("!dakim")));
	given(server, CONNECT, std::string(":lo1 432 !dakim :Erroneous nickname\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("1dakim")));
	given(server, CONNECT, std::string(":lo1 432 1dakim :Erroneous nickname\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("d!akim")));
	given(server, CONNECT, std::string(":lo1 432 d!akim :Erroneous nickname\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("dadadadadadada")));
	given(server, CONNECT, std::string(":lo1 432 dadadadadadada :Erroneous nickname\r"), std::string("dakim"));
	expect(Message(std::string(""), std::string("NICK"), std::string("lo.lo")));
	given(server, CONNECT, std::string(":lo1 432 lo.lo :Erroneous nickname\r"), std::string("dakim"));
}

TEST(UserSendNickMessageTest, NickOverlap)
{
	Server	server("111", "3000");
	Client	localServer(0, true);
	Client	remoteServer(0, true);
	Client	localUser(0, true);
	Client	remoteUser(0, true);

	localServer.setStatus(SERVER);
	localServer.setInfo(UPLINKSERVER, std::string("lo1"));
	localServer.setInfo(SERVERNAME, std::string("lo2"));
	localServer.setInfo(HOPCOUNT, std::string("1"));
	localServer.setInfo(SERVERINFO, std::string(":1"));
	server.sendClients[std::string("lo2")] = localServer;
	server.serverList[std::string("lo2")] = &server.sendClients[std::string("lo2")];

	remoteServer.setStatus(SERVER);
	remoteServer.setInfo(UPLINKSERVER, std::string("lo2"));
	remoteServer.setInfo(SERVERNAME, std::string("lo3"));
	remoteServer.setInfo(HOPCOUNT, std::string("2"));
	localServer.setInfo(SERVERINFO, std::string(":1"));
	server.sendClients[std::string("lo3")] = remoteServer;

	remoteUser.setStatus(USER);
	remoteUser.setInfo(HOSTNAME, std::string("lo2"));
	remoteUser.setInfo(NICK, std::string("da"));
	remoteUser.setInfo(ADDRESS, std::string("127.0.0.10"));
	remoteUser.setInfo(USERNAME, std::string("dak"));
	remoteUser.setInfo(REALNAME, std::string("deok"));
	server.sendClients[std::string("da")] = remoteUser;

	localUser.setStatus(USER);
	localUser.setInfo(HOSTNAME, std::string("lo1"));
	localUser.setInfo(NICK, std::string("d"));
	localUser.setInfo(ADDRESS, std::string("127.0.0.10"));
	localUser.setInfo(USERNAME, std::string("dak"));
	localUser.setInfo(REALNAME, std::string("deok"));
	server.sendClients[std::string("d")] = localUser;
	server.clientList[std::string("d")] = &server.sendClients[std::string("d")];

	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("dakim")));
	given(server, CONNECT, std::string(":lo1 433 dakim dakim :Nickname already in use\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("lo2")));
	given(server, CONNECT, std::string(":lo1 433 dakim lo2 :Nickname already in use\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("lo3")));
	given(server, CONNECT, std::string(":lo1 433 dakim lo3 :Nickname already in use\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("da")));
	given(server, CONNECT, std::string(":lo1 433 dakim da :Nickname already in use\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("d")));
	given(server, CONNECT, std::string(":lo1 433 dakim d :Nickname already in use\r"), std::string("dakim"));
	expect(Message(std::string(":dakim"), std::string("NICK"), std::string("lo1")));
	given(server, CONNECT, std::string(":lo1 433 dakim lo1 :Nickname already in use\r"), std::string("dakim"));
}

TEST(UserSendNickMessageTest, RegisterNickWithPrefix)
{
	int		i;
	int		fd[4];
	char	*result;
	Server	server("111", "3000");
	Client	*otherServer;
	Client	*anotherServer;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
	{
		otherServer = new Client(fd[1], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		otherServer->setInfo(SERVERNAME, std::string("lo2"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string(":1"));
		server.sendClients[std::string("lo2")] = *otherServer;
		server.serverList[std::string("lo2")] = &server.sendClients[std::string("lo2")];

		anotherServer = new Client(fd[3], true);
		anotherServer->setStatus(SERVER);
		anotherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		anotherServer->setInfo(SERVERNAME, std::string("lo3"));
		anotherServer->setInfo(HOPCOUNT, std::string("1"));
		anotherServer->setInfo(SERVERINFO, std::string(":1"));
		server.sendClients[std::string("lo3")] = *anotherServer;
		server.serverList[std::string("lo3")] = &server.sendClients[std::string("lo3")];

		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 0);
		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 0);
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string("deok")));
		given(server, CONNECT, std::string(":dakim!~dak@lo1 NICK :deok\r"), std::string("deok"));
		CHECK_EQUAL(server.sendClients.count(std::string("dakim")), 0);
		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 1);
		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim NICK :deok\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim NICK :deok\r"));
		free(result);
		delete otherServer;
		delete anotherServer;
		i = -1;
		while (++i < 4)
			close(fd[i]);
	}
}

TEST(UserSendNickMessageTest, RegisterNickWithOutPrefix)
{
	int		i;
	int		fd[4];
	char	*result;
	Server	server("111", "3000");
	Client	*otherServer;
	Client	*anotherServer;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
	{
		otherServer = new Client(fd[1], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		otherServer->setInfo(SERVERNAME, std::string("lo2"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string(":1"));
		server.sendClients[std::string("lo2")] = *otherServer;
		server.serverList[std::string("lo2")] = &server.sendClients[std::string("lo2")];

		anotherServer = new Client(fd[3], true);
		anotherServer->setStatus(SERVER);
		anotherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		anotherServer->setInfo(SERVERNAME, std::string("lo3"));
		anotherServer->setInfo(HOPCOUNT, std::string("1"));
		anotherServer->setInfo(SERVERINFO, std::string(":1"));
		server.sendClients[std::string("lo3")] = *anotherServer;
		server.serverList[std::string("lo3")] = &server.sendClients[std::string("lo3")];

		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 0);
		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 0);
		expect(Message(std::string(""), std::string("NICK"), std::string("deok")));
		given(server, CONNECT, std::string(":dakim!~dak@lo1 NICK :deok\r"), std::string("deok"));
		CHECK_EQUAL(server.sendClients.count(std::string("dakim")), 0);
		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 1);
		CHECK_EQUAL(server.sendClients.count(std::string("deok")), 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim NICK :deok\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim NICK :deok\r"));
		free(result);
		delete otherServer;
		delete anotherServer;
		i = -1;
		while (++i < 4)
			close(fd[i]);
	}
}

TEST_GROUP(ServerSendNickMessageTest)
{
	Client		*client;
	Message		sendMessage;

	void			expect(Message message, int fd)
	{
		client = new Client(fd, true);
		client->setStatus(SERVER);
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(SERVERNAME, std::string("lo2"));
		client->setInfo(HOPCOUNT, std::string("1"));
		client->setInfo(SERVERINFO, std::string("sexy server"));
		sendMessage = message;
	}
	void			given(Server &server, int connection,
					std::string nick, unsigned long sendClientNum)
	{
		server.prefix = std::string(":lo1");
		server.serverName = std::string("lo1");
		server.sendClients[client->getInfo(SERVERNAME)] = *client;
		server.serverList[client->getInfo(SERVERNAME)] = &server.sendClients[client->getInfo(SERVERNAME)];
		CHECK_EQUAL(connection, server.nickHandler(sendMessage, client));
		CHECK_EQUAL(client->getStatus(), SERVER);
		CHECK_EQUAL(client->getInfo(UPLINKSERVER), std::string("lo1"));
		CHECK_EQUAL(client->getInfo(SERVERNAME), std::string("lo2"));
		CHECK_EQUAL(client->getInfo(HOPCOUNT), std::string("1"));
		CHECK_EQUAL(client->getInfo(SERVERINFO), std::string("sexy server"));
		CHECK_EQUAL(server.sendClients.count(nick), sendClientNum);
		CHECK_EQUAL(server.clientList.count(nick), 0);
		delete client;
	}
};

TEST(ServerSendNickMessageTest, PrefixError)
{
	int		fd[2];
	char	*result;
	Client	*otherServer;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"sdfss\"\r"));
		free(result);
		expect(Message(std::string(":sdfss"), std::string("NICK"), std::string("dakim :1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"sdfss\"\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string("dakim :1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"dakim\"\r"));
		free(result);
		otherServer = new Client(1, true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		otherServer->setInfo(SERVERNAME, std::string("lo3"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.serverName = std::string("lo3");
		server.sendClients[otherServer->getInfo(SERVERNAME)] = *otherServer;
		server.serverList[otherServer->getInfo(SERVERNAME)] = &server.sendClients[otherServer->getInfo(SERVERNAME)];
		expect(Message(std::string(":lo3"), std::string("NICK"), std::string("")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"lo3\"\r"));
		free(result);
		delete otherServer;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendNickMessageTest, ParameterError)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(""), std::string("NICK"), std::string("")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 NICK :Syntax error\r"));
		free(result);
		expect(Message(std::string(""), std::string("NICK"), std::string("dakim")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 NICK :Syntax error\r"));
		free(result);
		expect(Message(std::string(""), std::string("NICK"), std::string("dakim 1 1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 NICK :Syntax error\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendNickMessageTest, LocalUserNickOverlap)
{
	int		fd[4];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
	{
		client = new Client(fd[3], true);
		client->setStatus(USER);
		client->setInfo(HOSTNAME, std::string("lo1"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(USERNAME, std::string("dak"));
		client->setInfo(REALNAME, std::string("deok"));
		server.acceptClients[fd[3]] = *client;
		server.sendClients[std::string("dakim")] = *client;
		server.clientList[std::string("dakim")] = &server.sendClients[std::string("dakim")];
		expect(Message(std::string(""), std::string("NICK"), std::string("dakim :1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		CHECK_EQUAL(server.acceptClients.count(fd[3]), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL dakim :Nick collision\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :Nick collision\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 436 dakim :Nick collision\r"));
		free(result);
		delete client;
		close(fd[0]);
		close(fd[1]);
		close(fd[2]);
	}
}

TEST(ServerSendNickMessageTest, RemoteUserNickOverlap)
{
	int		fd[4];
	char	*result;
	Client	*otherServer;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
	{
		otherServer = new Client(fd[3], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		otherServer->setInfo(SERVERNAME, std::string("lo3"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[otherServer->getInfo(SERVERNAME)] = *otherServer;
		server.serverList[otherServer->getInfo(SERVERNAME)] = &server.sendClients[otherServer->getInfo(SERVERNAME)];
		client = new Client(fd[3], true);
		client->setStatus(USER);
		client->setInfo(HOSTNAME, std::string("lo3"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(USERNAME, std::string("dak"));
		client->setInfo(REALNAME, std::string("deok"));
		server.sendClients[std::string("dakim")] = *client;
		expect(Message(std::string(""), std::string("NICK"), std::string("dakim :1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL dakim :Nick collision\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL dakim :Nick collision\r"));
		free(result);
		delete otherServer;
		delete client;
		close(fd[0]);
		close(fd[1]);
		close(fd[2]);
		close(fd[3]);
	}
}


TEST(ServerSendNickMessageTest, RemoteUserNickAndServerNameOverlap)
{
	int		fd[6];
	char	*result;
	Client	*otherServer;
	Client	*anotherServer;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		otherServer = new Client(fd[3], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		otherServer->setInfo(SERVERNAME, std::string("lo3"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[otherServer->getInfo(SERVERNAME)] = *otherServer;
		server.serverList[otherServer->getInfo(SERVERNAME)] = &server.sendClients[otherServer->getInfo(SERVERNAME)];
		anotherServer = new Client(fd[5], true);
		anotherServer->setStatus(SERVER);
		anotherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		anotherServer->setInfo(SERVERNAME, std::string("lo4"));
		anotherServer->setInfo(HOPCOUNT, std::string("1"));
		anotherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[anotherServer->getInfo(SERVERNAME)] = *anotherServer;
		server.serverList[anotherServer->getInfo(SERVERNAME)] = &server.sendClients[anotherServer->getInfo(SERVERNAME)];
		client = new Client(fd[3], true);
		client->setStatus(USER);
		client->setInfo(HOSTNAME, std::string("lo3"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(USERNAME, std::string("dak"));
		client->setInfo(REALNAME, std::string("deok"));
		server.sendClients[std::string("dakim")] = *client;
		expect(Message(std::string(""), std::string("NICK"), std::string("lo1 :1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL lo1 :Nick collision\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 483 lo1 :You can't kill a server!\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL lo1 :Nick collision\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL lo1 :Nick collision\r"));
		free(result);
		delete otherServer;
		delete anotherServer;
		delete client;
		close(fd[0]);
		close(fd[1]);
		close(fd[2]);
		close(fd[3]);
		close(fd[4]);
		close(fd[5]);
	}
}

TEST(ServerSendNickMessageTest, RemoteUserNickAndOtherServerNameOverlap)
{
	int		fd[6];
	char	*result;
	Client	*otherServer;
	Client	*anotherServer;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		otherServer = new Client(fd[3], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		otherServer->setInfo(SERVERNAME, std::string("lo3"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[otherServer->getInfo(SERVERNAME)] = *otherServer;
		server.serverList[otherServer->getInfo(SERVERNAME)] = &server.sendClients[otherServer->getInfo(SERVERNAME)];
		anotherServer = new Client(fd[5], true);
		anotherServer->setStatus(SERVER);
		anotherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		anotherServer->setInfo(SERVERNAME, std::string("lo4"));
		anotherServer->setInfo(HOPCOUNT, std::string("1"));
		anotherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[anotherServer->getInfo(SERVERNAME)] = *anotherServer;
		server.serverList[anotherServer->getInfo(SERVERNAME)] = &server.sendClients[anotherServer->getInfo(SERVERNAME)];
		client = new Client(fd[3], true);
		client->setStatus(USER);
		client->setInfo(HOSTNAME, std::string("lo3"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(USERNAME, std::string("dak"));
		client->setInfo(REALNAME, std::string("deok"));
		server.sendClients[std::string("dakim")] = *client;
		expect(Message(std::string(""), std::string("NICK"), std::string("lo4 :1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL lo4 :Nick collision\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 483 lo4 :You can't kill a server!\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL lo4 :Nick collision\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 KILL lo4 :Nick collision\r"));
		free(result);
		delete otherServer;
		delete anotherServer;
		delete client;
		close(fd[0]);
		close(fd[1]);
		close(fd[2]);
		close(fd[3]);
		close(fd[4]);
		close(fd[5]);
	}
}

TEST(ServerSendNickMessageTest, RegisterUser)
{
	Server	server("111", "3000");

	expect(Message(std::string(""), std::string("NICK"), std::string("dakim :1")), 1);
	given(server, CONNECT, std::string("dakim"), 1);
	CHECK_EQUAL(server.sendClients[std::string("dakim")].getStatus(), UNKNOWN);
	CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
	CHECK_EQUAL(server.clientList.count(std::string("dakim")), 0);
}

TEST_GROUP(ServerResendNickMessageNotUserTest)
{
	Client		*client;
	Client		*setUser;
	Message		sendMessage;

	void			expect(Message message, int fd)
	{
		client = new Client(fd, true);
		client->setStatus(SERVER);
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(SERVERNAME, std::string("lo2"));
		client->setInfo(HOPCOUNT, std::string("1"));
		client->setInfo(SERVERINFO, std::string("sexy server"));
		setUser = new Client(fd, true);
		setUser->setInfo(NICK, std::string("dakim"));
		sendMessage = message;
	}
	void			given(Server &server, int connection,
					std::string nick, unsigned long sendClientNum, unsigned long previousClientNum)
	{
		server.prefix = std::string(":lo1");
		server.serverName = std::string("lo1");
		server.sendClients[client->getInfo(SERVERNAME)] = *client;
		server.serverList[client->getInfo(SERVERNAME)] = &server.sendClients[client->getInfo(SERVERNAME)];
		server.sendClients[setUser->getInfo(NICK)] = *setUser;
		CHECK_EQUAL(connection, server.nickHandler(sendMessage, client));
		CHECK_EQUAL(client->getStatus(), SERVER);
		CHECK_EQUAL(client->getInfo(UPLINKSERVER), std::string("lo1"));
		CHECK_EQUAL(client->getInfo(SERVERNAME), std::string("lo2"));
		CHECK_EQUAL(client->getInfo(HOPCOUNT), std::string("1"));
		CHECK_EQUAL(client->getInfo(SERVERINFO), std::string("sexy server"));
		CHECK_EQUAL(server.sendClients.count(std::string("dakim")), previousClientNum);
		CHECK_EQUAL(server.sendClients.count(nick), sendClientNum);
		CHECK_EQUAL(server.clientList.count(nick), 0);
		delete client;
		delete setUser;
	}
};

TEST(ServerResendNickMessageNotUserTest, PrefixError)
{
	int		fd[2];
	char	*result;
	Message	sendMessage;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":sdsdfsdf"), std::string("NICK"), std::string(":deok")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"sdsdfsdf\"\r"));
		free(result);
		expect(Message(std::string(":lo1"), std::string("NICK"), std::string(":deok")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"lo1\"\r"));
		free(result);
		expect(Message(std::string(":lo2"), std::string("NICK"), std::string(":deok")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"lo2\"\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerResendNickMessageNotUserTest, ParameterSizeError)
{
	int		fd[2];
	char	*result;
	Message	sendMessage;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string("")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 NICK :Syntax error\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string("1 1")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 NICK :Syntax error\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string("1 1 1")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 NICK :Syntax error\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerResendNickMessageNotUserTest, PrefixIsServer)
{
	int		fd[2];
	char	*result;
	Client	*localServer;
	Client	*remoteServer;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		localServer = new Client(0, true);
		localServer->setStatus(SERVER);
		localServer->setInfo(UPLINKSERVER, std::string("lo1"));
		localServer->setInfo(SERVERNAME, std::string("lo3"));
		localServer->setInfo(HOPCOUNT, std::string("1"));
		localServer->setInfo(SERVERINFO, std::string("123"));
		server.sendClients[localServer->getInfo(SERVERNAME)] = *localServer;
		server.serverList[localServer->getInfo(SERVERNAME)] = &server.sendClients[localServer->getInfo(SERVERNAME)];
		remoteServer = new Client(0, true);
		remoteServer->setStatus(SERVER);
		remoteServer->setInfo(UPLINKSERVER, std::string("lo3"));
		remoteServer->setInfo(SERVERNAME, std::string("lo4"));
		remoteServer->setInfo(HOPCOUNT, std::string("2"));
		remoteServer->setInfo(SERVERINFO, std::string("123"));
		server.sendClients[remoteServer->getInfo(SERVERNAME)] = *remoteServer;
		expect(Message(std::string(":lo4"), std::string("NICK"), std::string(":deok")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invaild prefix \"lo4\"\r"));
		free(result);
		delete localServer;
		delete remoteServer;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerResendNickMessageNotUserTest, ParameterFormError)
{
	int		fd[2];
	char	*result;
	Message	sendMessage;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string("deok")), fd[1]);
		given(server, CONNECT, std::string("deok"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 NICK :Syntax error\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerResendNickMessageNotUserTest, NotVaildNick)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string(":!de")), fd[1]);
		given(server, CONNECT, std::string("!de"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 432 !de lo2 :Erroneous nickname\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string(":sdfsdffssfde")), fd[1]);
		given(server, CONNECT, std::string("sdfsdffssfde"), 0, 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 432 sdfsdffssfde lo2 :Erroneous nickname\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerResendNickMessageNotUserTest, NickOverlap)
{
	int		fd[2];
	char	*result;
	Client	*localUser;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		localUser = new Client(0, true);
		localUser->setStatus(USER);
		localUser->setInfo(UPLINKSERVER, std::string("lo1"));
		localUser->setInfo(NICK, std::string("deok"));
		localUser->setInfo(REALNAME, std::string("deok"));
		localUser->setInfo(USERNAME, std::string("deok"));
		localUser->setInfo(ADDRESS, std::string("127.0.0.1"));
		server.sendClients[localUser->getInfo(NICK)] = *localUser;
		server.clientList[localUser->getInfo(NICK)] = &server.sendClients[localUser->getInfo(NICK)];

		expect(Message(std::string(":dakim"), std::string("NICK"), std::string(":deok")), fd[1]);
		given(server, CONNECT, std::string("lo1"), 0, 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 433 dakim deok :Nickname already in use\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string(":lo1")), fd[1]);
		given(server, CONNECT, std::string("lo1"), 0, 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 433 dakim lo1 :Nickname already in use\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("NICK"), std::string(":lo2")), fd[1]);
		given(server, CONNECT, std::string("lo1"), 0, 1);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 433 dakim lo2 :Nickname already in use\r"));
		free(result);
		delete localUser;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerResendNickMessageNotUserTest, RegisterNick)
{
	Server	server("111", "3000");

	expect(Message(std::string(":dakim"), std::string("NICK"), std::string(":deok")), 1);
	given(server, CONNECT, std::string("deok"), 1, 0);
	CHECK_EQUAL(server.sendClients[std::string("deok")].getStatus(), UNKNOWN);
	CHECK_EQUAL(server.sendClients[std::string("deok")].getInfo(NICK), std::string("deok"));
	CHECK_EQUAL(server.clientList.count(std::string("dakim")), 0);
}
