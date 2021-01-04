#include "Test.hpp"

TEST_GROUP(MessageParsing)
{
	std::string messageStr;
	void			expect(std::string input)
	{
		messageStr = input;
	}
	void			given(std::string prefix, std::string command, std::vector<std::string> &parameters, std::string expectStr)
	{
		int			i;
		int			index;
		Message message(messageStr);
		i = -1;
		index = parameters.size();
		CHECK_EQUAL(message.getTotalMessage(), expectStr);
		CHECK_EQUAL(message.getPrefix(), prefix);
		CHECK_EQUAL(message.getCommand(), command);
		CHECK_EQUAL(message.getParameters().size(), parameters.size());
		while (++i < index)
		{
			CHECK_EQUAL(message.getParameter(i), parameters[i]);
		}
	}
};

TEST(MessageParsing, OnlyNewLine)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	expect("command 123\n");
	given("", "COMMAND", vector, "COMMAND 123\r\n");
}

TEST(MessageParsing, NotPrefix)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	expect("command 123\r");
	given("", "COMMAND", vector, "COMMAND 123\r\n");
}

TEST(MessageParsing, NotPrefixParameters)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	vector.push_back("123");
	expect("command 123 123\r");
	given("", "COMMAND", vector, "COMMAND 123 123\r\n");
}

TEST(MessageParsing, NotPrefixParametersLastParameter)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	vector.push_back("123");
	vector.push_back(":123 123");
	expect("command 123 123 :123 123\r");
	given("", "COMMAND", vector, "COMMAND 123 123 :123 123\r\n");
}

TEST(MessageParsing, Prefix)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	expect(":prefix command 123\r");
	given(":prefix", "COMMAND", vector, ":prefix COMMAND 123\r\n");
}

TEST(MessageParsing, PrefixParameters)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	vector.push_back("123");
	expect(":prefix command 123 123\r");
	given(":prefix", "COMMAND", vector, ":prefix COMMAND 123 123\r\n");
}

TEST(MessageParsing, PrefixParametersLastParameter)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	vector.push_back("123");
	vector.push_back(":123 123");
	expect(":prefix command 123 123 :123 123\r");
	given(":prefix", "COMMAND", vector, ":prefix COMMAND 123 123 :123 123\r\n");
}

TEST(MessageParsing, PrefixNotParameters)
{
	std::vector<std::string> vector;
	expect(":prefix command\r");
	given(":prefix", "COMMAND", vector, ":prefix COMMAND\r\n");
}

TEST(MessageParsing, NotPrefixNotParameters)
{
	std::vector<std::string> vector;
	expect("command\r");
	given("", "COMMAND", vector, "COMMAND\r\n");
}

TEST(MessageParsing, LastParameterOne)
{
	std::vector<std::string> vector;
	vector.push_back("guest");
	vector.push_back("tolmoon");
	vector.push_back("tolsun");
	vector.push_back(":Ronnie Reagan");
	expect("USER guest tolmoon tolsun :Ronnie Reagan\r");
	given("", "USER", vector, "USER guest tolmoon tolsun :Ronnie Reagan\r\n");
}

TEST(MessageParsing, LastParameterTwo)
{
	std::vector<std::string> vector;
	vector.push_back("csd.bu.edu");
	vector.push_back("5");
	vector.push_back(":BU Central Server");
	expect(":tolsun.oulu.fi SERVER csd.bu.edu 5 :BU Central Server\r");
	given(":tolsun.oulu.fi", "SERVER", vector, ":tolsun.oulu.fi SERVER csd.bu.edu 5 :BU Central Server\r\n");
}

TEST(MessageParsing, LastParameterThree)
{
	std::vector<std::string> vector;
	vector.push_back(":Gone to have lunch");
	expect("QUIT :Gone to have lunch\r");
	given("", "QUIT", vector, "QUIT :Gone to have lunch\r\n");
}

// TODO channel관리 하면서 파싱 기준 판단 필요
// TEST(MessageParsing, LastParameterFour)
// {
// 	std::vector<std::string> vector;
// 	vector.push_back("#foo");
// 	vector.push_back("&bar");
// 	vector.push_back("fubar");
// 	expect("JOIN #foo,&bar fubar\r");
// 	given("", "JOIN", vector);
// }

// TEST(MessageParsing, LastParameterFive)
// {
// 	std::vector<std::string> vector;
// 	vector.push_back("#foo");
// 	vector.push_back("#bar");
// 	vector.push_back("fubar");
// 	vector.push_back("foobar");
// 	expect("JOIN #foo,#bar fubar,foobar\r");
// 	given("", "JOIN", vector);
// }

TEST(MessageParsing, LastParameterSix)
{
	std::vector<std::string> vector;
	vector.push_back("#Twilight_zone");
	expect(":WiZ JOIN #Twilight_zone\r");
	given(":WiZ", "JOIN", vector, ":WiZ JOIN #Twilight_zone\r\n");
}

TEST(MessageParsing, LowerCaseCommand)
{
	std::vector<std::string> vector;
	vector.push_back("#Twilight_zone");
	expect(":WiZ join #Twilight_zone\r");
	given(":WiZ", "JOIN", vector, ":WiZ JOIN #Twilight_zone\r\n");
}
