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

	server.sendClients["dakim"] = Client(0, true);
	expect("", "NICK dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 433 * dakim :Nickname already in use\r\n", CONNECT, "", UNKNOWN);
	expect("da", "NICK dakim\r\n", true, UNKNOWN, "", "");
	given(server, ":localhost.3000 433 da dakim :Nickname already in use\r\n", CONNECT, "da", UNKNOWN);
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
	void		given(Server &server, int expectConnection, std::string nick, ClientStatus status)
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
