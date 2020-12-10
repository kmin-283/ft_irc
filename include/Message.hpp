#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "utils.hpp"

class							Message
{
private:
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	parameters;
	void						skipSpace(std::string::const_iterator &iterator, const std::string &message);
	void						setString(std::string &target, std::string::const_iterator &iterator, const std::string &message);
	void						setParameters(std::string::const_iterator &iterator, const std::string &message);
	void						setVector(std::vector<std::string> &vector, std::string &parameter);
public:
									Message(const std::string message);
									~Message(void);
	std::string						getPrefix(void) const;
	std::string						getCommand(void) const;
	std::string						getParameter(int index) const;
	std::vector<std::string>		getParameters(void) const;
};

#endif
