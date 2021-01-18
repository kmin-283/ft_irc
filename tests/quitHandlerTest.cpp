#include "Test.hpp"

TEST_GROUP(QuitHandlerTest)
{
	Client	*client;
	Message	message;

	void			expect(Client *clientPtr, Message sendMessage)
	{
		message = sendMessage;
		client = clientPtr;
	}
	void			given(Server &server, int connection)
	{
		CHECK_EQUAL(server.quitHandler(message, client), connection);
	}
};

TEST(QuitHandlerTest, LocalUserPrefixError)
{
	int		fd[2];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		client->setStatus(USER);
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(REALNAME, std::string("deok"));
		client->setInfo(USERNAME, std::string("deok"));
		server.serverName = std::string("lo1");
		server.ipAddress = std::string("127.0.0.1");
		server.sendClients[client->getInfo(NICK)] = *client;
		server.clientList[client->getInfo(NICK)] = &server.sendClients[client->getInfo(NICK)];
		expect(client, Message(std::string(":"), std::string("QUIT"), std::string("")));
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"\"\r"));
		free(result);
		expect(client, Message(std::string(":aa"), std::string("QUIT"), std::string("")));
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \"aa\"\r"));
		free(result);
		expect(client, Message(std::string("::"), std::string("QUIT"), std::string("")));
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Invalid prefix \":\"\r"));
		free(result);
		delete(client);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(QuitHandlerTest, LocalUserParameterError)
{
	int		fd[2];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		client->setStatus(USER);
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(REALNAME, std::string("deok"));
		client->setInfo(USERNAME, std::string("deok"));
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		server.ipAddress = std::string("127.0.0.1");
		server.sendClients[client->getInfo(NICK)] = *client;
		server.clientList[client->getInfo(NICK)] = &server.sendClients[client->getInfo(NICK)];
		expect(client, Message(std::string(""), std::string("QUIT"), std::string("1 1")));
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 dakim QUIT :Syntax error\r"));
		free(result);
		expect(client, Message(std::string(""), std::string("QUIT"), std::string("1 1 1")));
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 dakim QUIT :Syntax error\r"));
		free(result);
		expect(client, Message(std::string(""), std::string("QUIT"), std::string("1 1 1 1")));
		given(server, CONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string(":lo1 461 dakim QUIT :Syntax error\r"));
		free(result);
		delete(client);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(QuitHandlerTest, QuitLocalUser)
{
	int		fd[2];
	char	*result;
	Client	*client;
	Server	server("111", "3000");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		client->setStatus(USER);
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(REALNAME, std::string("deok"));
		client->setInfo(USERNAME, std::string("deok"));
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		server.ipAddress = std::string("127.0.0.1");
		server.sendClients[client->getInfo(NICK)] = *client;
		server.clientList[client->getInfo(NICK)] = &server.sendClients[client->getInfo(NICK)];
		expect(client, Message(std::string(""), std::string("QUIT"), std::string("")));
		given(server, DISCONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Closing connection\r"));
		free(result);
		expect(client, Message(std::string(""), std::string("QUIT"), std::string("1")));
		given(server, DISCONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :\"1\"\r"));
		free(result);
		expect(client, Message(std::string(""), std::string("QUIT"), std::string(":123123")));
		given(server, DISCONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :\"123123\"\r"));
		free(result);
		delete(client);
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(QuitHandlerTest, QuitLocalUserBroadcast)
{
	int		i;
	int		fd[6];
	char	*result;
	Client	*client;
	Client	*otherServer;
	Client	*anotherServer;
	Server	server("111", "3000");

	if (pipe(fd) != -1 && pipe(fd + 2) != -1 && pipe(fd + 4) != -1)
	{
		client = new Client(fd[1], true);
		client->setStatus(USER);
		client->setInfo(UPLINKSERVER, std::string("lo1"));
		client->setInfo(NICK, std::string("dakim"));
		client->setInfo(ADDRESS, std::string("127.0.0.1"));
		client->setInfo(REALNAME, std::string("deok"));
		client->setInfo(USERNAME, std::string("deok"));
		server.serverName = std::string("lo1");
		server.prefix = std::string(":lo1");
		server.ipAddress = std::string("127.0.0.1");
		server.sendClients[client->getInfo(NICK)] = *client;
		server.clientList[client->getInfo(NICK)] = &server.sendClients[client->getInfo(NICK)];
		otherServer = new Client(fd[3], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(SERVERNAME, std::string("lo2"));
		otherServer->setInfo(HOPCOUNT, std::string("2"));
		otherServer->setInfo(SERVERINFO, std::string("sexy server"));
		otherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		server.sendClients[otherServer->getInfo(SERVERNAME)] = *otherServer;
		server.serverList[otherServer->getInfo(SERVERNAME)] = &server.sendClients[otherServer->getInfo(SERVERNAME)];
		anotherServer = new Client(fd[5], true);
		anotherServer->setStatus(SERVER);
		anotherServer->setInfo(SERVERNAME, std::string("lo3"));
		anotherServer->setInfo(HOPCOUNT, std::string("2"));
		anotherServer->setInfo(SERVERINFO, std::string("sexy server"));
		anotherServer->setInfo(UPLINKSERVER, std::string("lo1"));
		server.sendClients[anotherServer->getInfo(SERVERNAME)] = *anotherServer;
		server.serverList[anotherServer->getInfo(SERVERNAME)] = &server.sendClients[anotherServer->getInfo(SERVERNAME)];
		expect(client, Message(std::string(""), std::string("QUIT"), std::string("")));
		given(server, DISCONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :Closing connection\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :Client closed connection\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :Client closed connection\r"));
		free(result);
		expect(client, Message(std::string(""), std::string("QUIT"), std::string("1")));
		given(server, DISCONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :\"1\"\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :\"1\"\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :\"1\"\r"));
		free(result);
		expect(client, Message(std::string(""), std::string("QUIT"), std::string(":123123")));
		given(server, DISCONNECT);
		get_next_line(fd[0], &result);
		CHECK_EQUAL(std::string(result), std::string("ERROR :\"123123\"\r"));
		free(result);
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :\"123123\"\r"));
		free(result);
		get_next_line(fd[4], &result);
		CHECK_EQUAL(std::string(result), std::string(":dakim QUIT :\"123123\"\r"));
		free(result);
		delete(client);
		delete(otherServer);
		delete(anotherServer);
		i = -1;
		while (++i < 6)
			close(fd[i]);
	}
}
