#include "Test.hpp"

TEST_GROUP(UnknownClientSendOperMessage)
{
	Client		*client;
	Message		sendMessage;

	void			expect(Message message, Client *clientPtr)
	{
		client = clientPtr;
		sendMessage = message;
	}
	void			given(Server &server, int connection)
	{
		server.acceptClients[client->getFd()] = *client;
		CHECK_EQUAL(connection, server.operHandler(sendMessage, client));
	}
};

TEST(UnknownClientSendOperMessage, UnauthorizedClientTest)
{
	int		fd[2];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		client = new Client(fd[1], false);
		expect(Message(":lo1 OPER oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		delete client;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UnknownClientSendOperMessage, AuthorizedClientTest)
{
	int		fd[2];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		client = new Client(fd[1], true);
		expect(Message(":lo1 OPER oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		delete client;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UnknownClientSendOperMessage, NickMessageRegisteredClientTest)
{
	int		fd[2];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		client = new Client(fd[1], true);
		client->setInfo(NICK, std::string("dakim"));
		server.sendClients[client->getInfo(NICK)] = *client;
		server.clientList[client->getInfo(NICK)] = &server.sendClients[client->getInfo(NICK)];
		expect(Message(":lo1 OPER oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 dakim :Connection not registered\r"));
		free(result);
		expect(Message("OPER\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 dakim :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 dakim :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 dakim :Connection not registered\r"));
		free(result);
		delete client;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UnknownClientSendOperMessage, UserMessageRegisteredClientTest)
{
	int		fd[2];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		client = new Client(fd[1], true);
		client->setInfo(USERNAME, std::string("dakim"));
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(REALNAME, std::string("dakim"));
		expect(Message(":lo1 OPER oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper oper oper\r\n"), client);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 * :Connection not registered\r"));
		free(result);
		delete client;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST_GROUP(ServerSendOperMessage)
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
	void			given(Server &server, int connection)
	{
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		server.acceptClients[client->getFd()] = *client;
		server.sendClients[client->getInfo(SERVERNAME)] = *client;
		server.serverList[client->getInfo(SERVERNAME)] = &server.sendClients[client->getInfo(SERVERNAME)];
		CHECK_EQUAL(connection, server.operHandler(sendMessage, client));
		delete client;
	}
};

TEST(ServerSendOperMessage, PrefixEmptyTest)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message("OPER\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper oper\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		expect(Message("OPER oper oper oper\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendOperMessage, PrefixNotEmptyTest)
{
	int		fd[2];
	char	*result;
	Client	*remoteUser;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		remoteUser = new Client(0, true);
		remoteUser->setStatus(USER);
		remoteUser->setInfo(UPLINKSERVER, std::string("lo2"));
		remoteUser->setInfo(NICK, std::string("dakim"));
		remoteUser->setInfo(ADDRESS, std::string("127.0.0.1"));
		remoteUser->setInfo(USERNAME, std::string("dakim"));
		remoteUser->setInfo(REALNAME, std::string("dakim"));
		server.sendClients[remoteUser->getInfo(NICK)] = *remoteUser;
		expect(Message(":dakim OPER\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		expect(Message(":dakim OPER oper\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		expect(Message(":dakim OPER oper oper\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		expect(Message(":dakim OPER oper oper oper\r\n"), fd[1]);
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 451 lo2 :Connection not registered\r"));
		free(result);
		delete remoteUser;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST_GROUP(UserSendOperMessage)
{
	Client		*client;
	Message		sendMessage;

	void			expect(Message message, int fd, std::string mode)
	{
		client = new Client(fd, true);
		client->setStatus(USER);
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(USERNAME, std::string("dakim"));
		client->setInfo(REALNAME, std::string("dakim"));
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(USERMODE, mode);
		sendMessage = message;
	}
	void			given(Server &server, std::string mode, int connection)
	{
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		server.acceptClients[client->getFd()] = *client;
		server.sendClients[client->getInfo(SERVERNAME)] = *client;
		server.clientList[client->getInfo(SERVERNAME)] = &server.sendClients[client->getInfo(SERVERNAME)];
		CHECK_EQUAL(connection, server.operHandler(sendMessage, &server.acceptClients[client->getFd()]));
		CHECK_EQUAL(server.acceptClients[client->getFd()].getInfo(USERMODE), mode);
		CHECK_EQUAL(server.sendClients[client->getInfo(NICK)].getInfo(USERMODE), mode);
		delete client;
	}
};

TEST(UserSendOperMessage, PrefixTest)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(": OPER oper oper\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"\"\r"));
		free(result);
		expect(Message(":d OPER oper oper\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"d\"\r"));
		free(result);
		expect(Message(":dd OPER oper oper\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"dd\"\r"));
		free(result);
		expect(Message(":lo1 OPER oper oper\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"lo1\"\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UserSendOperMessage, ParameterError)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message("OPER\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 dakim OPER :Syntax error\r"));
		free(result);
		expect(Message("OPER oper\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 dakim OPER :Syntax error\r"));
		free(result);
		expect(Message("OPER oper oper oper\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 dakim OPER :Syntax error\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UserSendOperMessage, OperatorNameError)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message("OPER invalid oper\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 491 dakim :Invalid operator host\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UserSendOperMessage, OperatorPassError)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message("OPER oper invalid\r\n"), fd[1], "");
		given(server, "", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 464 dakim :Invalid password\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UserSendOperMessage, RegisterOperMessage)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message("OPER oper oper\r\n"), fd[1], "");
		given(server, "o", CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 MODE dakim :+o\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 381 dakim :You are now an IRC Operator\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(UserSendOperMessage, RegisterOperBroadcast)
{
	int		i;
	int		fd[6];
	char	*result;
	Client	*otherServer;
	Client	*anotherServer;
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
		anotherServer->setInfo(UPLINKSERVER, std::string("lo2"));
		anotherServer->setInfo(SERVERNAME, std::string("lo4"));
		anotherServer->setInfo(HOPCOUNT, std::string("2"));
		anotherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[anotherServer->getInfo(SERVERNAME)] = *anotherServer;
		server.serverList[anotherServer->getInfo(SERVERNAME)] = &server.sendClients[anotherServer->getInfo(SERVERNAME)];

		expect(Message("OPER oper oper\r\n"), fd[1], "");
		given(server, "o", CONNECT);

		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim MODE dakim :+o\r"));
		free(result);

		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim MODE dakim :+o\r"));
		free(result);

		delete otherServer;
		delete anotherServer;
		i = -1;
		while (++i < 6)
			close(fd[i]);
	}
}
