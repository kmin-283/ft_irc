#include "ft_hpp.hpp"

TEST_GROUP(ServerHandlerFailTest)
{
	int			*fd;
	Client		*target;
	Message		*message;
	char		*result;
	std::string	resultStr;

	void			expect(int *fdPtr, Message *messagePtr, Client *clientPtr)
	{
		fd = fdPtr;
		target = clientPtr;
		message = messagePtr;
	}
	void			given(Server &server, std::string expectMessage)
	{
		result = NULL;
		if (target && message)
		{
			server.serverHandler(*message, target);
			get_next_line(fd[0], &result);
			resultStr = std::string(result) + "\n";
			CHECK_EQUAL(resultStr, expectMessage);
			free(result);
		}
	}
};

/**
 * 1. PASS 인증 한 경우
 * 	1-1. 인자가 제대로 들어온 경우
 * 		1-1-1. 서버 명 중복
 * 		=> ERROR :ID "localhost.6670" already registered
 * 		=> 연결 해제
 * 		1-1-2. 서버 명 중복 x
 * 			1-1-2-1. 직접 연결된 서버인 경우
 * 			=> :irc.example.net SERVER irc.example.net 1 :Server Info Text
 * 			1-1-2-2. 직접 연결되지 않은 서버인 경우
 * 			=> :irc.example.net SERVER localhost.6671 2 3 :123123
 * 				1-1-2-2-1. prefix가 맞지 않는경우
 * 				1-1-2-2-2. prefix가 맞는 경우
 * 	1-2. 인자가 제대로 들어오지 않은 경우
 * 	=> 461에러 리턴
 * 2. PASS 인증 하지 않은 경우 v
 * => 461
 * => 함수 종료
 **/

TEST(ServerHandlerFailTest, NotAuthorized)
{
	int			fd[2];
	Server		server("123", "3000");
	Client		*client;
	Message		*message = new Message("SERVER lo.lo 1 :1\r\n");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], false);
		expect(fd, message, client);
		given(server, ":localhost.3000 461 * SERVER :Syntax error\r\n");
		if (UNKNOWN == target->getStatus())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete client;
		delete message;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerHandlerFailTest, PrefixNotMatch)
{
	int			fd[2];
	Server		server("123", "3000");
	Client		*client;
	Message		*message = new Message(":dsdf.dfsf SERVER lo.lo 1 :1\r\n");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], false);
		expect(fd, message, client);
		given(server, ":localhost.3000 461 * SERVER :Syntax error\r\n");
		if (UNKNOWN == target->getStatus())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete client;
		delete message;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerHandlerFailTest, ServerNameFormNotMatch)
{
	int			fd[2];
	Server		server("123", "3000");
	Client		*client;
	Message		*message = new Message("SERVER lolo 1 :1\r\n");

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], false);
		expect(fd, message, client);
		given(server, ":localhost.3000 461 * SERVER :Syntax error\r\n");
		if (UNKNOWN == target->getStatus())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete client;
		delete message;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerHandlerFailTest, ParameterNotMatch)
{
	int			fd[2];
	Server		server("123", "3000");
	Message		*message;
	Client		*client;

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], false);
		message = new Message("SERVER\r\n");
		expect(fd, message, client);
		given(server, ":localhost.3000 461 * SERVER :Syntax error\r\n");
		if (UNKNOWN == target->getStatus())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete message;
		message = new Message("SERVER lolo\r\n");
		given(server, ":localhost.3000 461 * SERVER :Syntax error\r\n");
		if (UNKNOWN == target->getStatus())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete message;
		message = new Message("SERVER lolo 1\r\n");
		given(server, ":localhost.3000 461 * SERVER :Syntax error\r\n");
		if (UNKNOWN == target->getStatus())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete client;
		delete message;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerHandlerFailTest, ServerNameOverlapOne)
{
	int			fd[2];
	Server		server("123", "3000");
	Message		*message;
	Client		*client;
	Client		*otherServer = new Client(0, true);

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		server.acceptClients[fd[1]] = *client;
		server.sendClients["lo.lo"] = *otherServer;
		message = new Message("SERVER lo.lo 1 :1\r\n");
		expect(fd, message, client);
		given(server, "ERROR  :ID lo.lo already registered\r\n");
		if (server.acceptClients.find(fd[1]) == server.acceptClients.end())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete message;
		delete client;
		delete otherServer;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST(ServerHandlerFailTest, ServerNameOverlapTwo)
{
	int			fd[2];
	Server		server("123", "3000");
	Message		*message;
	Client		*client;

	if (pipe(fd) != -1)
	{
		client = new Client(fd[1], true);
		server.acceptClients[fd[1]] = *client;
		message = new Message("SERVER localhost.3000 1 :1\r\n");
		expect(fd, message, client);
		given(server, "ERROR  :ID localhost.3000 already registered\r\n");
		if (server.acceptClients.find(fd[1]) == server.acceptClients.end())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		delete message;
		delete client;
		close(fd[0]);
		close(fd[1]);
	}
}

TEST_GROUP(ServerHandlerSuccessTest)
{
};

// TEST(ServerHandlerTest, LocalServer)
// {
// 	int			fd[4];
// 	Server		server("123", "3000");
// 	Message		*message;
// 	Client		*client;
// 	Client		*otherServer;
// 	char		*result;

// 	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
// 	{
// 		client = new Client(fd[1], true);
// 		otherServer = new Client(fd[3], true);
// 		message = new Message("SERVER lo.lo 1 :1\r\n");
// 		otherServer->setStatus(SERVER);
// 		// server.serverMap["other"] = *otherServer;
// 		expect(fd, message, client);
// 		given(server, ":localhost.3000 server connection accepted\r\n");
// 		if (SERVER == target->getStatus())
// 			CHECK_EQUAL(1, 1);
// 		else
// 			CHECK_EQUAL(1, 0);
// 		if (server.acceptClients.find(client->getFd()) != server.acceptClients.end())
// 			CHECK_EQUAL(1, 1);
// 		else
// 			CHECK_EQUAL(1, 0);
// 		if (server.sendClients.find("lo.lo") != server.sendClients.end())
// 			CHECK_EQUAL(1, 1);
// 		else
// 			CHECK_EQUAL(1, 0);
// 		get_next_line(fd[0], &result);
// 		// TODO :123123의미 파악 못함
// 		std::string expectStr = std::string(result) + "\n";
// 		CHECK_EQUAL(expectStr, std::string(":localhost.3000 SERVER lo.lo 2 3 :1\r\n"));
// 		free(result);
// 		close(fd[0]);
// 		close(fd[1]);
// 		close(fd[2]);
// 		close(fd[3]);
// 		delete message;
// 		delete otherServer;
// 		delete client;
// 	}
// }

TEST(ServerHandlerTest, RemoteServer)
{
	int			fd[4];
	Server		server("123", "3000");
	Client		*parentServer;
	Client		*otherServer;
	Message		*message;
	char		*result;

	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
	{
		parentServer = new Client(fd[1], true);
		parentServer->setStatus(SERVER);
		parentServer->setInfo(Message("SERVER first.server 1 :123\r\n"));
		server.sendClients["first.server"] = *parentServer;
		server.acceptClients[fd[1]] = *parentServer;
		otherServer = new Client(fd[3], true);
		otherServer->setStatus(SERVER);
		otherServer->setInfo(Message("SERVER second.server 1 :1\r\n"));
		server.sendClients["second.server"] = *otherServer;
		server.acceptClients[fd[3]] = *otherServer;
		message = new Message(":fist.server SERVER third.server 2 0 :12\r\n");
		expect(fd, message, parentServer);
		given(server, ":localhost.3000 SERVER second.server 2 0 :123\r\n");
		get_next_line(fd[2], &result);
		CHECK_EQUAL(std::string(std::string(result) + "\n"), std::string(":localhost.3000 SERVER third.server 3 0 :12\r\n"));
		if (server.sendClients.find("third.server") != server.sendClients.end())
			CHECK_EQUAL(1, 1);
		else
			CHECK_EQUAL(1, 0);
		free(result);
		delete parentServer;
		delete otherServer;
		delete message;
	}
}

// TEST(ServerHandlerTest, RemoteServerPrefixNotMatch)
// {
// 	int			fd[4];
// 	Server		server("123", "3000");
// 	Client		*parentServer;
// 	Message		*message;
// 	char		*result;

// 	if (pipe(fd) != -1 && pipe(fd + 2) != -1)
// 	{
// 		parentServer = new Client(fd[1], true);
// 		parentServer->setStatus(SERVER);
// 		parentServer->setInfo(Message("SERVER first.server 1 :123\r\n"));
// 		server.sendClients["first.server"] = *parentServer;
// 		server.acceptClients[fd[1]] = *parentServer;
// 		server.sendClients["second.server"] = *otherServer;
// 		server.acceptClients[fd[3]] = *otherServer;
// 		message = new Message(":fist.server SERVER third.server 2 0 :12\r\n");
// 		expect(fd, message, parentServer);
// 		given(server, ":localhost.3000 SERVER second.server 2 0 :123\r\n");
// 		get_next_line(fd[2], &result);
// 		CHECK_EQUAL(std::string(std::string(result) + "\n"), std::string(":localhost.3000 SERVER third.server 3 0 :12\r\n"));
// 		if (server.sendClients.find("third.server") != server.sendClients.end())
// 			CHECK_EQUAL(1, 1);
// 		else
// 			CHECK_EQUAL(1, 0);
// 		free(result);
// 		delete parentServer;
// 		delete otherServer;
// 		delete message;
// 	}
// }