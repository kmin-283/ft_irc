#include "Test.hpp"

TEST_GROUP(RegisterUser)
{
	Server		*server;
	Client		*client;

	void		expect(Server *serverPtr, Client *clientPtr)
	{
		client = clientPtr;
		server = serverPtr;
	}
	void		given(std::string nick, std::string userName, std::string realName,
		std::string address, std::string hostName, ClientStatus status)
	{
		if (client != NULL)
		{
			CHECK_EQUAL(client->getStatus(), status);
			CHECK_EQUAL(client->getInfo(NICK), nick);
			CHECK_EQUAL(client->getInfo(USERNAME), userName);
			CHECK_EQUAL(client->getInfo(ADDRESS), address);
			CHECK_EQUAL(client->getInfo(REALNAME), realName);
			CHECK_EQUAL(client->getInfo(HOSTNAME), hostName);
			CHECK_EQUAL(server->sendClients["dakim"].getInfo(NICK), nick);
			CHECK_EQUAL(server->sendClients["dakim"].getInfo(USERNAME), userName);
			CHECK_EQUAL(server->sendClients["dakim"].getInfo(ADDRESS), address);
			CHECK_EQUAL(server->sendClients["dakim"].getInfo(REALNAME), realName);
			CHECK_EQUAL(server->sendClients["dakim"].getInfo(HOSTNAME), hostName);
		}
	}
};

TEST(RegisterUser, NickFirst)
{
	int			fd[2];
	char		*result;
	Client		*client;
	std::string	expectStr;
	Server		server("111", "3000");
	Message		nickMessage;
	Message		userMessage;

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		server.prefix = std::string(":localhost.3000");
		server.serverName = std::string("localhost.3000");
		server.version = std::string("ircserv.1.0");
		server.startTime = std::time(NULL);
		server.userMode = std::string("abBcCFiIoqrRswx");
		server.channelMode = std::string("abehiIklmMnoOPqQrRstvVz");
		server.motdDir = std::string("./ft_irc.motd");
		server.ipAddress = std::string("127.0.0.1");
		nickMessage = Message(std::string("NICK dakim\r\n"));
		userMessage = Message(std::string("USER da 1 1 :kkiii kkii\r\n"));
		expect(&server, client);
		server.nickHandler(nickMessage, client);
		server.userHandler(userMessage, client);
		if (server.clientList.find("dakim") != server.clientList.end())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		given(std::string("dakim"), std::string("da"), std::string("kkiii kkii"),
		std::string("127.0.0.1"), std::string("localhost.3000"), USER);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 001 dakim :Welcome to the Internet Relay Network dakim!~da@localhost.3000\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 002 dakim :Your host is localhost.3000, running version ircserv.1.0\r"));
		free(result);
		get_next_line(fd[0], &result);
		std::string expectStr = std::string(":localhost.3000 003 dakim :This server has been started ");
		expectStr += getTimestamp(server.startTime, false);
		expectStr += "\r";
		CHECK_EQUAL(std::string(result), expectStr);
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
		close(fd[0]);
		close(fd[1]);
		delete client;
	}
}

TEST(RegisterUser, UserFirst)
{
	int			fd[2];
	char		*result;
	Client		*client;
	std::string	expectStr;
	Server		server("111", "3000");
	Message		nickMessage;
	Message		userMessage;

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		server.prefix = std::string(":localhost.3000");
		server.serverName = std::string("localhost.3000");
		server.version = std::string("ircserv.1.0");
		server.startTime = std::time(NULL);
		server.userMode = std::string("abBcCFiIoqrRswx");
		server.channelMode = std::string("abehiIklmMnoOPqQrRstvVz");
		server.motdDir = std::string("./ft_irc.motd");
		server.ipAddress = std::string("127.0.0.1");
		nickMessage = Message(std::string("NICK dakim\r\n"));
		userMessage = Message(std::string("USER da 1 1 :kkiii kkii\r\n"));
		expect(&server, client);
		server.userHandler(userMessage, client);
		server.nickHandler(nickMessage, client);
		if (server.clientList.find("dakim") != server.clientList.end())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		given(std::string("dakim"), std::string("da"), std::string("kkiii kkii"),
		std::string("127.0.0.1"), std::string("localhost.3000"), USER);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 001 dakim :Welcome to the Internet Relay Network dakim!~da@localhost.3000\r"));
		free(result);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":localhost.3000 002 dakim :Your host is localhost.3000, running version ircserv.1.0\r"));
		free(result);
		get_next_line(fd[0], &result);
		std::string expectStr = std::string(":localhost.3000 003 dakim :This server has been started ");
		expectStr += getTimestamp(server.startTime, false);
		expectStr += "\r";
		CHECK_EQUAL(std::string(result), expectStr);
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
		close(fd[0]);
		close(fd[1]);
		delete client;
	}
}

TEST(RegisterUser, BroadCastUserFirst)
{
	int			i;
	int			fd[6];
	char		*result;
	Client		*client;
	Client		*otherServer;
	Client		*anotherServer;
	std::string	expectStr;
	Server		server("111", "3000");
	Message		nickMessage;
	Message		userMessage;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		client = new Client(fd[1], true);
		otherServer = new Client(fd[3], true);
		anotherServer = new Client(fd[5], true);
		otherServer->setInfo(SERVERNAME, "otherServer");
		anotherServer->setInfo(SERVERNAME, "anotherServer");
		server.serverList["otherServer"] = otherServer;
		server.serverList["anotherServer"] = anotherServer;
		server.ipAddress = std::string("127.0.0.1");
		server.prefix = std::string(":localhost.3000");
		server.serverName = std::string("localhost.3000");
		server.version = std::string("ircserv.1.0");
		server.startTime = std::time(NULL);
		server.userMode = std::string("abBcCFiIoqrRswx");
		server.channelMode = std::string("abehiIklmMnoOPqQrRstvVz");
		server.motdDir = std::string("./ft_irc.motd");
		server.ipAddress = std::string("127.0.0.1");
		nickMessage = Message(std::string("NICK dakim\r\n"));
		userMessage = Message(std::string("USER da 1 1 :kkiii kkii\r\n"));
		expect(&server, client);
		server.userHandler(userMessage, client);
		server.nickHandler(nickMessage, client);
		if (server.clientList.find("dakim") != server.clientList.end())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		given(std::string("dakim"), std::string("da"), std::string("kkiii kkii"),
		std::string("127.0.0.1"), std::string("localhost.3000"), USER);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~da 127.0.0.1 localhost.3000 :kkiii kkii\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~da 127.0.0.1 localhost.3000 :kkiii kkii\r"));
		free(result);
		i = -1;
		while (fd[++i] < 6)
			close(fd[i]);
		delete client;
		delete otherServer;
		delete anotherServer;
	}
}

TEST(RegisterUser, BroadCastNickFirst)
{
	int			i;
	int			fd[6];
	char		*result;
	Client		*client;
	Client		*otherServer;
	Client		*anotherServer;
	std::string	expectStr;
	Server		server("111", "3000");
	Message		nickMessage;
	Message		userMessage;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		client = new Client(fd[1], true);
		otherServer = new Client(fd[3], true);
		anotherServer = new Client(fd[5], true);
		otherServer->setInfo(SERVERNAME, "otherServer");
		anotherServer->setInfo(SERVERNAME, "anotherServer");
		server.serverList["otherServer"] = otherServer;
		server.serverList["anotherServer"] = anotherServer;
		server.ipAddress = std::string("127.0.0.1");
		server.prefix = std::string(":localhost.3000");
		server.serverName = std::string("localhost.3000");
		server.version = std::string("ircserv.1.0");
		server.startTime = std::time(NULL);
		server.userMode = std::string("abBcCFiIoqrRswx");
		server.channelMode = std::string("abehiIklmMnoOPqQrRstvVz");
		server.motdDir = std::string("./ft_irc.motd");
		server.ipAddress = std::string("127.0.0.1");
		nickMessage = Message(std::string("NICK dakim\r\n"));
		userMessage = Message(std::string("USER da 1 1 :kkiii kkii\r\n"));
		expect(&server, client);
		server.nickHandler(nickMessage, client);
		server.userHandler(userMessage, client);
		if (server.clientList.find("dakim") != server.clientList.end())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		given(std::string("dakim"), std::string("da"), std::string("kkiii kkii"),
		std::string("127.0.0.1"), std::string("localhost.3000"), USER);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~da 127.0.0.1 localhost.3000 :kkiii kkii\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string("NICK dakim :1\r"));
		free(result); get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim USER ~da 127.0.0.1 localhost.3000 :kkiii kkii\r"));
		free(result);
		i = -1;
		while (fd[++i] < 6)
			close(fd[i]);
		delete client;
		delete otherServer;
		delete anotherServer;
	}
}
