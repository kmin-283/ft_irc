#include "Test.hpp"

TEST_GROUP(UserHandlerErrorTest)
{
	int			fd[2];
	char		*result;
	Client		*client;
	Message		message;
	int			connectionStatus;

	void		expect(Message sendMessage, bool authority, std::string nick, std::string userName)
	{
		client = NULL;
		if (pipe(fd) != -1)
		{
			client = new Client(fd[1], authority);
			client->setInfo(NICK, nick);
			client->setInfo(USERNAME, userName);
			client->setStatus(UNKNOWN);
			message = sendMessage;
		}
	}
	void		given(Server server, int expectStatus, std::string expectStr)
	{
		if (client != NULL)
		{
			connectionStatus = server.userHandler(message, client);
			get_next_line(fd[0], &result);
			CHECK_EQUAL(connectionStatus, expectStatus);
			CHECK_EQUAL(std::string(result), expectStr);
			free(result);
			delete client;
		}

	}
};

TEST(UserHandlerErrorTest, UnauthorizedUserFormatFalse)
{
	Server	server("111", "3000");

	server.prefix = std::string(":localhost.3000");
	expect(std::string("USER\r\n"), false, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1\r\n"), false, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1 1\r\n"), false, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1 1 1\r\n"), false, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1 1 1 1 1\r\n"), false, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
}

TEST(UserHandlerErrorTest, AuthorizedUserFormatFalse)
{
	Server	server("111", "3000");

	server.prefix = std::string(":localhost.3000");
	expect(std::string("USER\r\n"), true, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1\r\n"), true, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1 1\r\n"), true, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1 1 1\r\n"), true, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
	expect(std::string("USER 1 1 1 1 1\r\n"), true, "", "");
	given(server, CONNECT, std::string(":localhost.3000 461 * USER :Syntax error\r"));
}

TEST(UserHandlerErrorTest, NickRegisteredUserFormatFalse)
{
	Server	server("111", "3000");

	server.prefix = std::string(":localhost.3000");
	expect(std::string("USER\r\n"), true, "dakim", "");
	given(server, CONNECT, std::string(":localhost.3000 461 dakim USER :Syntax error\r"));
	expect(std::string("USER 1\r\n"), true, "dakim", "");
	given(server, CONNECT, std::string(":localhost.3000 461 dakim USER :Syntax error\r"));
	expect(std::string("USER 1 1\r\n"), true, "dakim", "");
	given(server, CONNECT, std::string(":localhost.3000 461 dakim USER :Syntax error\r"));
	expect(std::string("USER 1 1 1\r\n"), true, "dakim", "");
	given(server, CONNECT, std::string(":localhost.3000 461 dakim USER :Syntax error\r"));
	expect(std::string("USER 1 1 1 1 1\r\n"), true, "dakim", "");
	given(server, CONNECT, std::string(":localhost.3000 461 dakim USER :Syntax error\r"));
}

TEST(UserHandlerErrorTest, Unauthorized)
{
	Server	server("111", "3000");

	server.prefix = std::string(":localhost.3000");
	expect(std::string("USER 1 1 1 1\r\n"), false, "", "da");
	given(server, DISCONNECT, std::string("ERROR :Access denied: Bad password\r"));
}

TEST(UserHandlerErrorTest, UserRegistered)
{
	Server	server("111", "3000");

	server.prefix = std::string(":localhost.3000");
	expect(std::string("USER 1 1 1 1\r\n"), true, "", "da");
	given(server, CONNECT, std::string(":localhost.3000 462 :Unauthorized command (already registered)\r"));
}

TEST(UserHandlerErrorTest, NickRegisteredUserRegistered)
{
	Server	server("111", "3000");

	server.prefix = std::string(":localhost.3000");
	expect(std::string("USER 1 1 1 1\r\n"), true, "dakim", "da");
	given(server, CONNECT, std::string(":localhost.3000 462 :Unauthorized command (already registered)\r"));
}

TEST(UserHandlerErrorTest, InvailedUserName)
{
	Server	server("111", "3000");
	Message message;

	message.prefix = std::string("");
	message.command = std::string("USER");
	message.parameters.assign(4, "");
	message.parameters[1] = std::string("1");
	message.parameters[2] = std::string("1");
	message.parameters[3] = std::string("1");
	server.prefix = std::string("");
	message.parameters[0] = std::string("\rdakim");
	expect(message, true, "dakim", "");
	given(server, CONNECT, std::string("ERROR USER :Erroneous username\r"));
	message.parameters[0] = std::string("\ndakim");
	expect(message, true, "dakim", "");
	given(server, CONNECT, std::string("ERROR USER :Erroneous username\r"));
	message.parameters[0] = std::string("da kim");
	expect(message, true, "dakim", "");
	given(server, CONNECT, std::string("ERROR USER :Erroneous username\r"));
}

TEST_GROUP(UserHanderTest)
{
	Message		sendMessage;
	Client		*client;

	void		expect(Client *clientPtr, Message message)
	{
		client = clientPtr;
		sendMessage = message;
	}
	void		given(Server &server, std::string userName, std::string realName, std::string address,
				std::string hostName, int connectStatus, ClientStatus status)
	{
		if (client != NULL)
		{
			CHECK_EQUAL(connectStatus, server.userHandler(sendMessage, client));
			CHECK_EQUAL(client->getStatus(), status);
			CHECK_EQUAL(client->getInfo(USERNAME), userName);
			CHECK_EQUAL(client->getInfo(ADDRESS), address);
			CHECK_EQUAL(client->getInfo(REALNAME), realName);
			CHECK_EQUAL(client->getInfo(HOSTNAME), hostName);
		}
	}
};

TEST(UserHanderTest, RegisterUserOnly)
{
	int			fd[2];
	Server		server("111", "3000");
	Client		*client;
	Message		message;

	if (pipe(fd) != -1)
	{
		server.serverName = std::string("localhost.3000");
		server.ipAddress = std::string("127.0.0.1");
		client = new Client(fd[1], true);
		message = Message("USER da 1 1 :dakim\r\n");
		expect(client, message);
		given(server, std::string("da"), std::string("dakim"), std::string("127.0.0.1"),
		std::string("localhost.3000"), CONNECT, UNKNOWN);
		delete client;
		client = new Client(fd[1], true);
		message = Message("USER da 1 1 dakim\r\n");
		expect(client, message);
		given(server, std::string("da"), std::string("dakim"), std::string("127.0.0.1"),
		std::string("localhost.3000"), CONNECT, UNKNOWN);
		close(fd[0]);
		close(fd[1]);
		delete client;
	}
}

TEST(UserHanderTest, RegisterNickUser)
{
	int			fd[2];
	Server		server("111", "3000");
	Client		*client;
	Message		message;

	if (pipe(fd) != -1)
	{
		server.serverName = std::string("localhost.3000");
		server.motdDir = std::string("./ft_irc.motd");
		server.ipAddress = std::string("127.0.0.1");
		client = new Client(fd[1], true);
		message = Message("USER da 1 1 :dakim\r\n");
		server.nickHandler(Message(std::string("NICK dakim\r\n")), client);
		expect(client, message);
		given(server, std::string("da"), std::string("dakim"), std::string("127.0.0.1"),
		std::string("localhost.3000"), CONNECT, USER);
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(USERNAME), std::string("da"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(ADDRESS), std::string("127.0.0.1"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(REALNAME), std::string("dakim"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(HOSTNAME), std::string("localhost.3000"));
		close(fd[0]);
		close(fd[1]);
		delete client;
	}
}

TEST_GROUP(UserSendUserMessageTest)
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
			client->setInfo(HOSTNAME, std::string("localhost.3000"));
			client->setInfo(NICK, std::string("dakim"));
			client->setInfo(ADDRESS, std::string("127.0.0.1"));
			client->setInfo(USERNAME, std::string("dak"));
			client->setInfo(REALNAME, std::string("deok"));
			sendMessage = message;
		}
	}
	void			given(Server server, int connection, std::string expectStr)
	{
		if (client != NULL)
		{
			server.prefix = std::string(":localhost.3000");
			server.serverName = std::string("localhost.3000");
			server.sendClients["dakim"] = *client;
			server.clientList["dakim"] = &server.sendClients["dakim"];
			CHECK_EQUAL(connection, server.userHandler(sendMessage, client));
			get_next_line(fd[0], &result);
			CHECK_EQUAL(std::string(result), expectStr);
			free(result);
			CHECK_EQUAL(client->getInfo(HOSTNAME), std::string("localhost.3000"));
			CHECK_EQUAL(client->getInfo(NICK), std::string("dakim"));
			CHECK_EQUAL(client->getInfo(ADDRESS), std::string("127.0.0.1"));
			CHECK_EQUAL(client->getInfo(USERNAME), std::string("dak"));
			CHECK_EQUAL(client->getInfo(REALNAME), std::string("deok"));
			delete client;
		}
	}
};

TEST(UserSendUserMessageTest, PrefixError)
{
	Server server("111", "3000");

	expect(Message(std::string(":"), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"\"\r"));
	expect(Message(std::string(":d"), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"d\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d d")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d d")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d d d d d d d")));
	given(server, CONNECT, std::string("ERROR :Invalid prefix \"sdfss\"\r"));
}

TEST(UserSendUserMessageTest, ParameterError)
{
	Server server("111", "3000");

	expect(Message(std::string(""), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(""), std::string("USER"), std::string("d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(""), std::string("USER"), std::string("d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(""), std::string("USER"), std::string("d d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(""), std::string("USER"), std::string("d d d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(""), std::string("USER"), std::string("d d d d d d d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));

	expect(Message(std::string(":dakim"), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(":dakim"), std::string("USER"), std::string("d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(":dakim"), std::string("USER"), std::string("d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(":dakim"), std::string("USER"), std::string("d d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(":dakim"), std::string("USER"), std::string("d d d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
	expect(Message(std::string(":dakim"), std::string("USER"), std::string("d d d d d d d d")));
	given(server, CONNECT, std::string(":localhost.3000 462 dakim :Connection already registered\r"));
}


TEST_GROUP(ServerSendUserMessageNotUserTest)
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
					std::string nick, unsigned long sendClientNum)
	{
		server.prefix = std::string(":lo1");
		server.serverName = std::string("lo1");
		server.acceptClients[client->getFd()] = *client;
		server.sendClients[client->getInfo(SERVERNAME)] = *client;
		server.serverList[client->getInfo(SERVERNAME)] = &server.sendClients[client->getInfo(SERVERNAME)];
		server.sendClients[setUser->getInfo(NICK)] = *setUser;
		CHECK_EQUAL(connection, server.userHandler(sendMessage, client));
		CHECK_EQUAL(client->getStatus(), SERVER);
		CHECK_EQUAL(client->getInfo(UPLINKSERVER), std::string("lo1"));
		CHECK_EQUAL(client->getInfo(SERVERNAME), std::string("lo2"));
		CHECK_EQUAL(client->getInfo(HOPCOUNT), std::string("1"));
		CHECK_EQUAL(client->getInfo(SERVERINFO), std::string("sexy server"));
		CHECK_EQUAL(server.sendClients.count(nick), sendClientNum);
		CHECK_EQUAL(server.clientList.count(nick), 0);
		delete client;
		delete setUser;
	}
};

TEST(ServerSendUserMessageNotUserTest, PrefixError)
{
	int		fd[2];
	char	*result;
	Message	sendMessage;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(""), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"\"\r"));
		free(result);
		expect(Message(std::string(":"), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"\"\r"));
		free(result);
		expect(Message(std::string(":sdsdfsdf"), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"sdsdfsdf\"\r"));
		free(result);
		expect(Message(std::string(":lo1"), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"lo1\"\r"));
		free(result);
		expect(Message(std::string(":lo2"), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"lo2\"\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, PrefixIsServer)
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
		expect(Message(std::string(":lo4"), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"lo4\"\r"));
		free(result);
		delete localServer;
		delete remoteServer;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, PrefixIsLocalUser)
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
		remoteUser->setInfo(NICK, std::string("lo4"));
		remoteUser->setInfo(REALNAME, std::string("123"));
		remoteUser->setInfo(USERNAME, std::string("123"));
		remoteUser->setInfo(ADDRESS, std::string("123"));
		server.sendClients[remoteUser->getInfo(SERVERNAME)] = *remoteUser;
		expect(Message(std::string(":lo4"), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		delete remoteUser;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, PrefixIsRemoteUser)
{
	int		fd[2];
	char	*result;
	Client	*localServer;
	Client	*remoteUser;
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
		remoteUser = new Client(0, true);
		remoteUser->setStatus(USER);
		remoteUser->setInfo(UPLINKSERVER, std::string("lo3"));
		remoteUser->setInfo(NICK, std::string("lo4"));
		remoteUser->setInfo(REALNAME, std::string("123"));
		remoteUser->setInfo(USERNAME, std::string("123"));
		remoteUser->setInfo(ADDRESS, std::string("123"));
		server.sendClients[remoteUser->getInfo(SERVERNAME)] = *remoteUser;
		expect(Message(std::string(":lo4"), std::string("USER"), std::string("~deok 211.117.140.153 lo2 :sdfsdd")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		delete localServer;
		delete remoteUser;
		close(fd[0]);
		close(fd[1]);
	}
}


TEST(ServerSendUserMessageNotUserTest, ParameterSizeError)
{
	int		fd[2];
	char	*result;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 USER :Syntax error\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 USER :Syntax error\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("1 1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 USER :Syntax error\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("1 1 1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 USER :Syntax error\r"));
		free(result);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("1 1 1 1 1")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getInfo(NICK), std::string("dakim"));
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 lo2 USER :Syntax error\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

// TEST(ServerSendUserMessageNotUserTest, AddressError)
// {
// 	int		fd[2];
// 	Server	server("111", "3000");

// 	if (pipe(fd) != -1)
// 	{
// 		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok sdfsfsdfs lo2 :sdf")), fd[1]);
// 		given(server, CONNECT, std::string("dakim"), 1);
// 		CHECK_EQUAL(server.sendClients[std::string("dakim")].getStatus(), UNKNOWN);
// 		close(fd[0]);
// 		close(fd[1]);
// 	}
// }

TEST(ServerSendUserMessageNotUserTest, ServerNameError)
{
	int		fd[2];
	Client	*localClient;
	Client	*remoteClient;
	Client	*relatedServer;
	Client	*notRelatedServer;

	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		relatedServer = new Client(0, true);
		relatedServer->setStatus(SERVER);
		relatedServer->setInfo(UPLINKSERVER, std::string("lo2"));
		relatedServer->setInfo(SERVERNAME, std::string("lo3"));
		relatedServer->setInfo(HOPCOUNT, std::string("2"));
		relatedServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[relatedServer->getInfo(SERVERNAME)] = *relatedServer;
		notRelatedServer = new Client(0, true);
		notRelatedServer->setStatus(SERVER);
		notRelatedServer->setInfo(UPLINKSERVER, std::string("lo1"));
		notRelatedServer->setInfo(SERVERNAME, std::string("lo4"));
		notRelatedServer->setInfo(HOPCOUNT, std::string("1"));
		notRelatedServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[notRelatedServer->getInfo(SERVERNAME)] = *notRelatedServer;
		server.serverList[notRelatedServer->getInfo(SERVERNAME)] = &server.sendClients[notRelatedServer->getInfo(SERVERNAME)];
		localClient = new Client(0, true);
		localClient->setStatus(USER);
		localClient->setInfo(UPLINKSERVER, std::string("lo1"));
		localClient->setInfo(NICK, std::string("user1"));
		localClient->setInfo(USERNAME, std::string("user1"));
		localClient->setInfo(REALNAME, std::string("user1"));
		localClient->setInfo(ADDRESS, std::string("127.0.0.1"));
		server.sendClients[localClient->getInfo(NICK)] = *localClient;
		server.clientList[localClient->getInfo(NICK)] = &server.sendClients[localClient->getInfo(NICK)];
		remoteClient = new Client(0, true);
		remoteClient->setStatus(USER);
		remoteClient->setInfo(UPLINKSERVER, std::string("lo2"));
		remoteClient->setInfo(NICK, std::string("user2"));
		remoteClient->setInfo(USERNAME, std::string("user2"));
		remoteClient->setInfo(REALNAME, std::string("user2"));
		remoteClient->setInfo(ADDRESS, std::string("127.0.0.1"));
		server.sendClients[remoteClient->getInfo(NICK)] = *remoteClient;
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo1 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getStatus(), UNKNOWN);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo4 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getStatus(), UNKNOWN);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 user1 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getStatus(), UNKNOWN);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 user2 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getStatus(), UNKNOWN);
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 sdfsfsf :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		CHECK_EQUAL(server.sendClients[std::string("dakim")].getStatus(), UNKNOWN);
		delete localClient;
		delete remoteClient;
		delete relatedServer;
		delete notRelatedServer;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, RegisterUser)
{
	int		fd[2];
	char	*result;
	Client	*registerClient;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo2 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		registerClient = &server.sendClients[std::string("dakim")];
		CHECK_EQUAL(registerClient->getStatus(), USER);
		CHECK_EQUAL(registerClient->getInfo(USERNAME), std::string("deok"));
		CHECK_EQUAL(registerClient->getInfo(REALNAME), std::string("sdf"));
		CHECK_EQUAL(registerClient->getInfo(ADDRESS), std::string("127.0.0.1"));
		server.userHandler(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo2 :sdf")), &server.sendClients[std::string("lo2")]);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, RegisterUserRemoteServer)
{
	int		fd[2];
	char	*result;
	Client	*relatedServer;
	Client	*registerClient;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		relatedServer = new Client(fd[1], true);
		relatedServer->setStatus(SERVER);
		relatedServer->setInfo(UPLINKSERVER, std::string("lo2"));
		relatedServer->setInfo(SERVERNAME, std::string("lo3"));
		relatedServer->setInfo(HOPCOUNT, std::string("2"));
		relatedServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[relatedServer->getInfo(SERVERNAME)] = *relatedServer;
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo3 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		registerClient = &server.sendClients[std::string("dakim")];
		CHECK_EQUAL(registerClient->getStatus(), USER);
		CHECK_EQUAL(registerClient->getInfo(USERNAME), std::string("deok"));
		CHECK_EQUAL(registerClient->getInfo(REALNAME), std::string("sdf"));
		CHECK_EQUAL(registerClient->getInfo(ADDRESS), std::string("127.0.0.1"));
		server.userHandler(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo3 :sdf")), &server.sendClients[std::string("lo2")]);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		delete relatedServer;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, RegisterUserWithoutTilde)
{
	int		fd[2];
	char	*result;
	Client	*registerClient;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("deok 127.0.0.1 lo2 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		registerClient = &server.sendClients[std::string("dakim")];
		CHECK_EQUAL(registerClient->getStatus(), USER);
		CHECK_EQUAL(registerClient->getInfo(USERNAME), std::string("deok"));
		CHECK_EQUAL(registerClient->getInfo(REALNAME), std::string("sdf"));
		CHECK_EQUAL(registerClient->getInfo(ADDRESS), std::string("127.0.0.1"));
		server.userHandler(Message(std::string(":dakim"), std::string("USER"), std::string("deok 127.0.0.1 lo2 :sdf")), &server.sendClients[std::string("lo2")]);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, RegisterUserRemoteServerWithoutTilde)
{
	int		fd[2];
	char	*result;
	Client	*relatedServer;
	Client	*registerClient;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		relatedServer = new Client(fd[1], true);
		relatedServer->setStatus(SERVER);
		relatedServer->setInfo(UPLINKSERVER, std::string("lo2"));
		relatedServer->setInfo(SERVERNAME, std::string("lo3"));
		relatedServer->setInfo(HOPCOUNT, std::string("2"));
		relatedServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[relatedServer->getInfo(SERVERNAME)] = *relatedServer;
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("deok 127.0.0.1 lo3 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		registerClient = &server.sendClients[std::string("dakim")];
		CHECK_EQUAL(registerClient->getStatus(), USER);
		CHECK_EQUAL(registerClient->getInfo(USERNAME), std::string("deok"));
		CHECK_EQUAL(registerClient->getInfo(REALNAME), std::string("sdf"));
		CHECK_EQUAL(registerClient->getInfo(ADDRESS), std::string("127.0.0.1"));
		server.userHandler(Message(std::string(":dakim"), std::string("USER"), std::string("deok 127.0.0.1 lo3 :sdf")), &server.sendClients[std::string("lo2")]);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		delete relatedServer;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerSendUserMessageNotUserTest, RegisterUserBroadCast)
{
	int		i;
	int		fd[6];
	char	*result;
	Client	*registerClient;
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

		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo2 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		registerClient = &server.sendClients[std::string("dakim")];
		CHECK_EQUAL(registerClient->getStatus(), USER);
		CHECK_EQUAL(registerClient->getInfo(USERNAME), std::string("deok"));
		CHECK_EQUAL(registerClient->getInfo(REALNAME), std::string("sdf"));
		CHECK_EQUAL(registerClient->getInfo(ADDRESS), std::string("127.0.0.1"));

		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~deok 127.0.0.1 lo2 :sdf\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~deok 127.0.0.1 lo2 :sdf\r"));
		free(result);

		server.userHandler(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo2 :sdf")), &server.sendClients[std::string("lo2")]);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		delete otherServer;
		delete anotherServer;
		i = -1;
		while (++i < 6)
			close(fd[i]);
	}
}

TEST(ServerSendUserMessageNotUserTest, RegisterUserRemoteServerBroadCast)
{
	int		i;
	int		fd[6];
	char	*result;
	Client	*otherServer;
	Client	*anotherServer;
	Client	*relatedServer;
	Client	*registerClient;
	Server	server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		otherServer = new Client(fd[3], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		otherServer->setInfo(SERVERNAME, std::string("lo4"));
		otherServer->setInfo(HOPCOUNT, std::string("1"));
		otherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[otherServer->getInfo(SERVERNAME)] = *otherServer;
		server.serverList[otherServer->getInfo(SERVERNAME)] = &server.sendClients[otherServer->getInfo(SERVERNAME)];

		anotherServer = new Client(fd[5], true);
		anotherServer->setStatus(SERVER);
		anotherServer->setInfo(UPLINKSERVER, std::string("lo2"));
		anotherServer->setInfo(SERVERNAME, std::string("lo5"));
		anotherServer->setInfo(HOPCOUNT, std::string("2"));
		anotherServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[anotherServer->getInfo(SERVERNAME)] = *anotherServer;
		server.serverList[anotherServer->getInfo(SERVERNAME)] = &server.sendClients[anotherServer->getInfo(SERVERNAME)];

		relatedServer = new Client(fd[1], true);
		relatedServer->setStatus(SERVER);
		relatedServer->setInfo(UPLINKSERVER, std::string("lo2"));
		relatedServer->setInfo(SERVERNAME, std::string("lo3"));
		relatedServer->setInfo(HOPCOUNT, std::string("2"));
		relatedServer->setInfo(SERVERINFO, std::string("sexy server"));
		server.sendClients[relatedServer->getInfo(SERVERNAME)] = *relatedServer;
		expect(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo3 :sdf")), fd[1]);
		given(server, CONNECT, std::string("dakim"), 1);
		registerClient = &server.sendClients[std::string("dakim")];
		CHECK_EQUAL(registerClient->getStatus(), USER);
		CHECK_EQUAL(registerClient->getInfo(USERNAME), std::string("deok"));
		CHECK_EQUAL(registerClient->getInfo(REALNAME), std::string("sdf"));
		CHECK_EQUAL(registerClient->getInfo(ADDRESS), std::string("127.0.0.1"));

		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~deok 127.0.0.1 lo2 :sdf\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~deok 127.0.0.1 lo2 :sdf\r"));
		free(result);

		server.userHandler(Message(std::string(":dakim"), std::string("USER"), std::string("~deok 127.0.0.1 lo3 :sdf")), &server.sendClients[std::string("lo2")]);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 462 :Unauthorized command (already registered)\r"));
		free(result);
		delete relatedServer;
		delete otherServer;
		delete anotherServer;
		i = -1;
		while (++i < 6)
			close(fd[i]);
	}
}
