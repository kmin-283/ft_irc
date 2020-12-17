#include "ft_hpp.hpp"

TEST_GROUP(ServerHandlerTest)
{
	int			fd[2];
	Client		*target;
	Message		*message;
	char		*result;
	std::string	resultStr;

	void			expect(bool authority, std::string messageStr)
	{
		target = NULL;
		message = NULL;
		if (pipe(fd) != -1)
		{
			target = new Client(fd[1], authority);
			message = new Message(messageStr);
		}
	}
	void			given(Server &server, ClientStatus status, std::string expectMessage)
	{
		resultStr = NULL;
		if (target && message)
		{
			server.serverHandler(message, target);
			get_next_line(fd[0], &result);
			resultStr = std::string(result) + "\n";
			CHECK_EQUAL(resultStr, expectMessage);
			CHECK_EQUAL(status, client.getStatus());
			delete client;
			delete message;
			free(result);
			close(fd[0]);
			close(fd[1]);
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
 * 	1-2. 인자가 제대로 들어오지 않은 경우
 * 	=> 461에러 리턴
 * 2. PASS 인증 하지 않은 경우 v
 * => 461
 * => 함수 종료
 **/

TEST(ServerHandlerTest, NotAuthorized)
{
	Server server("123", "3000");
	expect(false, "SERVER lo.lo 1 :1");
	given(server, UNKNOWN, "localhost.3000 461 * SERVER :Syntax error\r\n");
}

TEST(ServerHandlerTest, ServerNameFormNotMatch)
{
	Server server("123", "3000");
	expect(true, "SERVER lolo 1 :1");
	given(server, UNKNOWN, "localhost.3000 461 * SERVER :Syntax error\r\n");
}

TEST(ServerHandlerTest, ParameterNotMatch)
{
	Server server("123", "3000");
	expect(true, "SERVER");
	given(server, UNKNOWN, "localhost.3000 461 * SERVER :Syntax error\r\n");
	expect(true, "SERVER lolo");
	given(server, UNKNOWN, "localhost.3000 461 * SERVER :Syntax error\r\n");
	expect(true, "SERVER lolo 1");
	given(server, UNKNOWN, "localhost.3000 461 * SERVER :Syntax error\r\n");
	expect(true, "SERVER lolo 1 :1");
	given(server, UNKNOWN, "localhost.3000 461 * SERVER :Syntax error\r\n");
}

TEST(ServerHandlerTest, ServerNameOverlap)
{
	Client *client = new Client(0, ture);
	Server server("123", "3000");
	server.sendClients["lo.lo"] = client;
	expect(server true, "SERVER lo.lo 1 :1");
	given(server, UNKNOWN, "Error :ID lo.lo already registered")
	delete client;
}

TEST(ServerHandlerTest, RemoteServer)
{

}

TEST(ServerHandlerTest, LocalServer)
{

}
