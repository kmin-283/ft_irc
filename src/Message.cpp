#include "Message.hpp"

Message::Message(const std::string message)
{
	std::string::const_iterator		messageIterator;
    // :prefix command param :param param
	messageIterator = message.begin();
	if (*messageIterator == ':')
		this->setString(this->prefix, messageIterator, message);
	this->setString(this->command, messageIterator, message);
	this->setParameters(messageIterator, message);
}

Message::~Message(void)
{
}

void						Message::skipSpace(std::string::const_iterator &iterator, const std::string &message)
{
	while (*iterator == ' ' && iterator != message.end())
		++iterator;
}

void						Message::setString(std::string &target, std::string::const_iterator &iterator, const std::string &message)
{
	target = "";
	while (*iterator != ' ' && *iterator != '\r' && iterator != message.end())
	{
		target += *iterator;
		++iterator;
	}
	this->skipSpace(iterator, message);
}

void						Message::setParameters(std::string::const_iterator &iterator, const std::string &message)
{
	std::string parameter;

	while (*iterator != '\r')
	{
		if (*iterator == ':')
		{
			parameter = "";
			while (*iterator != '\r' && iterator != message.end())
			{
				parameter += *iterator;
				++iterator;
			}
			this->setVector(this->parameters, parameter);
		}
		else
		{
			this->setString(parameter, iterator, message);

			this->setVector(this->parameters, parameter);
		}
	}
}

void						Message::setVector(std::vector<std::string> &vector, std::string &parameter)
{
	vector.push_back(parameter);
	parameter = "";
}

std::string					Message::getPrefix(void) const
{
	return (this->prefix);
}

std::string					Message::getCommand(void) const
{
	return (this->command);
}

std::string					Message::getParameter(int index) const
{
	return (this->parameters[index]);
}

std::vector<std::string>	Message::getParameters(void) const
{
	return (this->parameters);
}

void		printData(Message message)
{
	std::cout << "prefix = " << message.getPrefix() << std::endl;
	std::cout << "command = " << message.getCommand() << std::endl;
	int index = message.getParameters().size();
	int i;
	i = -1;
	while (++i < index)
	{
		std::cout << "parameters = " << message.getParameter(i) << std::endl;
	}
}