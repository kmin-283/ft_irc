// #include "Test.hpp"

// TEST_GROUP(LocalServerQuitErrorTest)
// {
// 	int			fd[2];
// 	char		*result;
// 	Client		*client;
// 	Message		message;
// 	int			connectionStatus;

// 	void		expect(Message sendMessage, bool authority)
// 	{
// 		client = NULL;
// 		if (pipe(fd) != -1)
// 		{
// 			client = new Client(fd[1], authority);
// 			client->setStatus(UNKNOWN);
// 			message = sendMessage;
// 		}
// 	}
// 	void		given(Server server, int expectStatus, std::string expectStr)
// 	{
// 		if (client != NULL)
// 		{
// 			connectionStatus = server.squitHandler(message, client);
// 			get_next_line(fd[0], &result);
// 			CHECK_EQUAL(connectionStatus, expectStatus);
// 			CHECK_EQUAL(std::string(result), expectStr);
// 			free(result);
// 			delete client;
// 		}
// 	}
// };

// TEST(LocalServerQuitErrorTest, test1)
// {
// 	Message		message;
// 	Server		server("111", "3000");

// 	message = Message("SQUIT\r\n");
// 	expect(message, false);
// 	given(server, CONNECT, std::string(":localhost.3000 461 * SQUIT :Syntax error\r"));
// 	message = Message("SQUIT localhost.123\r\n");
// 	expect(message, false);
// 	given(server, CONNECT, std::string(":localhost.3000 461 * SQUIT :Syntax error\r"));
// 	message = Message("SQUIT localhost.123 1\r\n");
// 	expect(message, false);
// 	given(server, CONNECT, std::string(":localhost.3000 461 * SQUIT :Syntax error\r"));
// 	message = Message("SQUIT localhost.123 sdfsdf :1\r\n");
// 	expect(message, false);
// 	given(server, CONNECT, std::string(":localhost.3000 461 * SQUIT :Syntax error\r"));
// 	message = Message("SQUIT localhost123 1 :1\r\n");
// 	expect(message, false);
// 	given(server, CONNECT, std::string(":localhost.3000 461 * SQUIT :Syntax error\r"));
// }
