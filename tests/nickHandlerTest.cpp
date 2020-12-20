#include "Test.hpp"

TEST_GROUP(NickHandlerTest)
{
	int			fd[2];
	char		*result;
	Client		*client;
	Message		*message;
	std::string	resultStr;
	int			connectStatus;

	void		expect(std::string nick, std::string messageStr, bool authority,
				ClientStatus status, std::string userName, std::string originNick,
				std::string serverName)
	{
		message = NULL;
		if (pipe(fd) != -1)
		{
			message = new Message(messageStr);
			client = new Client(fd[1], authority);
			client->setStatus(status);
			client->setInfo(CURRENTNICK, nick);
			client->setInfo(ORIGINNICK, originNick);
			client->setInfo(USERNAME, userName);
			client->setInfo(HOSTNAME, serverName);
		}
	}
	void		given(Server &server, std::string expectStr, int expectConnection,
				std::string currentNick, std::string originNick, ClientStatus status)
	{
		if (message != NULL && client != NULL)
		{
			connectStatus = server.nickHandler(*message, client);
			CHECK_EQUAL(connectStatus, expectConnection);
			get_next_line(fd[0], &result);
			resultStr = std::string(result) + std::string("\n");
			CHECK_EQUAL(resultStr, expectStr);
			CHECK_EQUAL(client->getInfo(CURRENTNICK), currentNick);
			CHECK_EQUAL(client->getInfo(ORIGINNICK), originNick);
			CHECK_EQUAL(client->getStatus(), status);
			close(fd[1]);
			close(fd[0]);
			delete message;
			delete client;
		}
	}
};

TEST(NickHandlerTest, UnknownParameterEmptyTest)
{
	Server		server("111", "3000");

	expect("", "NICK\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "", "", UNKNOWN);
	expect("", "NICK\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "", "", UNKNOWN);
	expect("dakim", "NICK\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "dakim", "", UNKNOWN);
	expect("dakim", "NICK\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 431 :No nickname given\r\n", CONNECT, "dakim", "", UNKNOWN);
}

TEST(NickHandlerTest, UnknownTooMuchParameterTest)
{
	Server		server("111", "3000");

	expect("", "NICK da da\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 461 * NICK :Syntax error\r\n", CONNECT, "", "", UNKNOWN);
	expect("", "NICK da da\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 461 * NICK :Syntax error\r\n", CONNECT, "", "", UNKNOWN);
	expect("dakim", "NICK da da\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 461 dakim NICK :Syntax error\r\n", CONNECT, "dakim", "", UNKNOWN);
	expect("dakim", "NICK da da\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 461 dakim NICK :Syntax error\r\n", CONNECT, "dakim", "", UNKNOWN);
}

TEST(NickHandlerTest, UnknownNickFormErrorTest)
{
	Server		server("111", "3000");

	expect("", "NICK !dakim\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("", "NICK !dakim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("dakim", "NICK !dakim\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);
	expect("dakim", "NICK !dakim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 !dakim :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);

	expect("", "NICK 1dakim\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("", "NICK 1dakim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("dakim", "NICK 1dakim\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);
	expect("dakim", "NICK 1dakim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 1dakim :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);

	expect("", "NICK d!akim\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("", "NICK d!akim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("dakim", "NICK d!akim\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);
	expect("dakim", "NICK d!akim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 d!akim :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);

	expect("", "NICK dadadadadadada\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("", "NICK dadadadadadada\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "", "", UNKNOWN);
	expect("dakim", "NICK dadadadadadada\r\n", false, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);
	expect("dakim", "NICK dadadadadadada\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 432 dadadadadadada :Erroneous nickname\r\n", CONNECT, "dakim", "", UNKNOWN);
}

TEST(NickHandlerTest, PassFail)
{
	Server		server("111", "3000");

	expect("", "NICK dakim\r\n", false, UNKNOWN, "", "", "");
	given(server, "ERROR :Access denied: Bad password\r\n", DISCONNECT, "", "", UNKNOWN);
}

TEST(NickHandlerTest, NickOverlap)
{
	Server		server("111", "3000");

	server.sendClients["dakim"] = Client(0, true);
	expect("", "NICK dakim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 433 * dakim :Nickname already in use\r\n", CONNECT, "", "", UNKNOWN);
	expect("da", "NICK dakim\r\n", true, UNKNOWN, "", "", "");
	given(server, ":localhost.3000 433 da dakim :Nickname already in use\r\n", CONNECT, "da", "", UNKNOWN);
}

TEST(NickHandlerTest, RegisterNick)
{
	Server		server("111", "3000");

	expect("", "NICK dakim\r\n", true, UNKNOWN, "", "", "");
	CHECK_EQUAL(server.sendClients.size(), 0);
	CHECK_EQUAL(server.clientList.size(), 0);
	given(server, "dakim :Nick registered\r\n", CONNECT, "dakim", "dakim", UNKNOWN);
	if (server.sendClients.find("dakim") != server.sendClients.end())
		CHECK_EQUAL(1,1);
	else
		CHECK_EQUAL(1, 0);
	CHECK_EQUAL(server.sendClients.size(), 1);
	CHECK_EQUAL(server.clientList.size(), 1);
	expect("dakim", "NICK dakim1\r\n", true, UNKNOWN, "", "dakim", "");
	given(server, "dakim1 :Nick registered\r\n", CONNECT, "dakim1", "dakim", UNKNOWN);
	if (server.sendClients.find("dakim") == server.sendClients.end())
		CHECK_EQUAL(1,1);
	else
		CHECK_EQUAL(1, 0);
	if (server.sendClients.find("dakim1") != server.sendClients.end())
		CHECK_EQUAL(1,1);
	else
		CHECK_EQUAL(1, 0);
	CHECK_EQUAL(server.sendClients.size(), 1);
	CHECK_EQUAL(server.clientList.size(), 1);
}

TEST(NickHandlerTest, RegisterUser)
{
	int			fd[2];
	char		*result;
	Client		*client;
	std::string	expectStr;
	Server		server("111", "3000");
	Message		message(std::string("NICK dakim\r\n"));

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		client->setInfo(USERNAME, "dakim");
		client->setInfo(HOSTNAME, "127.0.0.1");
		server.prefix = std::string(":localhost.3000");
		server.serverName = std::string("localhost.3000");
		server.version = std::string("ircserv.1.0");
		server.startTime = std::string("Thu Dec 17 2020 at 11:55:13 (UTC)");
		server.userMode = std::string("abBcCFiIoqrRswx");
		server.channelMode = std::string("abehiIklmMnoOPqQrRstvVz");
		server.motdDir = std::string("./ft_irc.motd");
		server.nickHandler(message, client);
		if (client->getStatus() == USER)
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 001 dakim :Welcome to the Internet Relay Network dakim!~dakim@localhost.3000\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 002 dakim :Your host is localhost.3000, running version ircserv.1.0\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 003 dakim :This server has been started Thu Dec 17 2020 at 11:55:13 (UTC)\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 004 dakim localhost.3000 ircserv.1.0 abBcCFiIoqrRswx abehiIklmMnoOPqQrRstvVz\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 251 dakim :There are 1 users and 0 services on 1 servers\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 254 dakim 0 :channels formed\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 255 dakim :I have 1 users, 0 services and 0 servers\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 375 dakim :- localhost.3000 message of the day\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 372 dakim :- **************************************************\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 372 dakim :- *             H    E    L    L    O              *\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 372 dakim :- **************************************************\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 372 dakim :- *  42cursus c5 ft_irc                            *\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 372 dakim :- *  Made by kmin seunkim dakim                    *\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 372 dakim :- **************************************************\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 376 dakim :End of MOTD command\r"));
		free(result);
		delete client;
	}
}
