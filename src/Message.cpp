#include "Message.hpp"

/**
 *
 *  :prefix command param :param param
 *
 *
 * 	:irc.example.net SERVER localhost.6671 2 3 :!23
 *
 **/

Message::Message(void) {}

Message::Message(const std::string &message)
 : totalMessage(message)
{
	std::string::const_iterator		messageIterator;
	messageIterator = message.begin();
	if (*messageIterator == ':')
		this->setString(this->prefix, messageIterator, message);
	this->setString(this->command, messageIterator, message);
	this->setParameters(messageIterator, message);
}

Message::Message(const std::string &prefix, const std::string &command, const std::string &parameters)
 : prefix(prefix), command(command)
{
	std::string	tmpParameters;
	std::string::const_iterator	iterator;

	tmpParameters = parameters;
	tmpParameters += CR_LF;
	iterator = tmpParameters.begin();
	this->setTotalMessage(prefix, command, parameters);
	this->setParameters(iterator, tmpParameters);
}

Message::~Message(void)
{
}

Message						&Message::operator=(const Message &message)
{
	this->totalMessage = message.totalMessage;
	this->prefix = message.prefix;
	this->command = message.command;
	this->parameters = message.parameters;
	return (*this);
}

void						Message::skipSpace(std::string::const_iterator &iterator, const std::string &message)
{
	while (*iterator == ' ' && iterator != message.end())
		++iterator;
}

void						Message::setTotalMessage(const std::string &prefix, const std::string &command, const std::string &parameters)
{
	this->totalMessage = "";
	this->totalMessage += prefix;
	if (prefix != "")
		this->totalMessage += " ";
	this->totalMessage += command;
	if (parameters != "")
		this->totalMessage += " ";
	this->totalMessage += parameters;
	this->totalMessage += CR_LF;
}

void						Message::setString(std::string &target, std::string::const_iterator &iterator, const std::string &message)
{
	target = "";
	while (*iterator != ' ' && *iterator != '\r' && *iterator != '\n' && iterator != message.end())
	{
		target += *iterator;
		++iterator;
	}
	this->skipSpace(iterator, message);
}

void						Message::setParameters(std::string::const_iterator &iterator, const std::string &message)
{
	std::string parameter;

	while (*iterator != '\r' && *iterator != '\n')
	{
		if (*iterator == ':')
		{
			parameter = "";
			while (*iterator != '\r' && *iterator != '\n' && iterator != message.end())
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

const std::string			&Message::getTotalMessage(void) const
{
	return (this->totalMessage);
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
