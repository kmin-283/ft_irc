#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "utils.hpp"

class							Message
{
private:
	std::string						totalMessage;
	std::string						prefix;
	std::string						command;
	std::vector<std::string>		parameters;
	void							skipSpace(std::string::const_iterator &iterator, const std::string &message);
	void							setString(std::string &target, std::string::const_iterator &iterator, const std::string &message);
	void							setParameters(std::string::const_iterator &iterator, const std::string &message);
	void							setVector(std::vector<std::string> &vector, std::string &parameter);
	void							setTotalMessage(const std::string &prefix, const std::string &message, const std::string &parameters);
public:
									Message(void);
									Message(const std::string &message);
									Message(const std::string &prefix, const std::string &command, const std::string &parameters);
									~Message(void);
	Message							&operator=(const Message &message);
	std::string						getPrefix(void) const;
	std::string						getCommand(void) const;
	std::string						getParameter(const int &index) const;
	std::vector<std::string>		getParameters(void) const;

	const std::string				&getTotalMessage(void) const;
};

#endif
