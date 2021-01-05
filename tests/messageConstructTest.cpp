#include "Test.hpp"

TEST_GROUP(MessageConstructTest)
{
	std::string prefixStr;
	std::string commandStr;
	std::string parametersStr;
	void			expect(std::string first, std::string second, std::string third)
	{
		prefixStr = first;
		commandStr = second;
		parametersStr = third;
	}
	void			given(std::string totalStr, std::vector<std::string> &parameters, std::string expectStr)
	{
		int			i;
		int			index;
		Message message(prefixStr, commandStr, parametersStr);
		i = -1;
		index = parameters.size();
		CHECK_EQUAL(message.getTotalMessage(), totalStr);
		CHECK_EQUAL(message.getPrefix(), prefixStr);
		CHECK_EQUAL(message.getCommand(), expectStr);
		CHECK_EQUAL(message.getParameters().size(), parameters.size());
		while (++i < index)
		{
			CHECK_EQUAL(message.getParameter(i), parameters[i]);
		}
	}
};

TEST(MessageConstructTest, NoPrefixUpper)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	expect("", "COMMAND", "123");
	given("COMMAND 123\r\n", vector, "COMMAND");
}

TEST(MessageConstructTest, PrefixUpper)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	expect(":123", "COMMAND", "123");
	given(":123 COMMAND 123\r\n", vector, "COMMAND");
}

TEST(MessageConstructTest, MultiParametersUpper)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	vector.push_back("123");
	vector.push_back(":123 123");
	expect(":123","COMMAND", "123 123 :123 123");
	given(":123 COMMAND 123 123 :123 123\r\n", vector, "COMMAND");
}

TEST(MessageConstructTest, NoPrefixLower)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	expect("", "command", "123");
	given("COMMAND 123\r\n", vector, "COMMAND");
}

TEST(MessageConstructTest, PrefixLower)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	expect(":123", "command", "123");
	given(":123 COMMAND 123\r\n", vector, "COMMAND");
}

TEST(MessageConstructTest, MultiParametersLower)
{
	std::vector<std::string> vector;
	vector.push_back("123");
	vector.push_back("123");
	vector.push_back(":123 123");
	expect(":123","command", "123 123 :123 123");
	given(":123 COMMAND 123 123 :123 123\r\n", vector, "COMMAND");
}
