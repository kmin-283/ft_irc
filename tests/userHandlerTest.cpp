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
				std::string hopCount, std::string hostName, int connectStatus, ClientStatus status)
	{
		if (client != NULL)
		{
			CHECK_EQUAL(connectStatus, server.userHandler(sendMessage, client));
			CHECK_EQUAL(client->getStatus(), status);
			CHECK_EQUAL(client->getInfo(USERNAME), userName);
			CHECK_EQUAL(client->getInfo(ADDRESS), address);
			CHECK_EQUAL(client->getInfo(REALNAME), realName);
			CHECK_EQUAL(client->getInfo(HOPCOUNT), hopCount);
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
		std::string("1"), std::string("localhost.3000"), CONNECT, UNKNOWN);
		delete client;
		client = new Client(fd[1], true);
		message = Message("USER da 1 1 dakim\r\n");
		expect(client, message);
		given(server, std::string("da"), std::string("dakim"), std::string("127.0.0.1"),
		std::string("1"), std::string("localhost.3000"), CONNECT, UNKNOWN);
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
		given(server, std::string("da"), std::string("dakim"), std::string("127.0.0.1"),
		std::string("1"), std::string("localhost.3000"), CONNECT, USER);
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(USERNAME), std::string("da"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(ADDRESS), std::string("127.0.0.1"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(REALNAME), std::string("dakim"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(HOPCOUNT), std::string("1"));
		CHECK_EQUAL(server.sendClients["dakim"].getInfo(HOSTNAME), std::string("localhost.3000"));
		close(fd[0]);
		close(fd[1]);
		delete client;
	}
}
