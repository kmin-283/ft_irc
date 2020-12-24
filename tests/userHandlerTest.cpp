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
		given(server, std::string("da"), std::string(":dakim"), std::string("127.0.0.1"),
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
		message = Message("USRE da 1 1 :dakim\r\n");
		server.nickHandler(Message(std::string("NICK dakim\r\n")), client);
		expect(client, message);
		given(server, std::string("da"), std::string(":dakim"), std::string("127.0.0.1"),
		std::string("localhost.3000"), CONNECT, USER);
		CHECK_EQUAL(client->getInfo(HOPCOUNT), std::string("1"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(USERNAME), std::string("da"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(ADDRESS), std::string("127.0.0.1"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(REALNAME), std::string(":dakim"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(HOPCOUNT), std::string("1"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(HOSTNAME), std::string("localhost.3000"));
		close(fd[0]);
		close(fd[1]);
		delete client;
	}
}

TEST_GROUP(RegisterUserAfterTest)
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
			client->setInfo(HOPCOUNT, std::string("1"));
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
			CHECK_EQUAL(client->getInfo(HOPCOUNT), std::string("1"));
			CHECK_EQUAL(client->getInfo(ADDRESS), std::string("127.0.0.1"));
			CHECK_EQUAL(client->getInfo(USERNAME), std::string("dak"));
			CHECK_EQUAL(client->getInfo(REALNAME), std::string("deok"));
			delete client;
		}
	}
};

TEST(RegisterUserAfterTest, PrefixError)
{
	Server server("111", "3000");

	expect(Message(std::string(":"), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"\"\r"));
	expect(Message(std::string(":d"), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \":d\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"));
	expect(Message(std::string(":sdfss"), std::string("USER"), std::string("d d d d d d d d")));
	given(server, CONNECT, std::string("ERROR :Invaild prefix \"sdfss\"\r"));
}

TEST(RegisterUserAfterTest, ParameterError)
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
