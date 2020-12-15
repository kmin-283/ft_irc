#include "Server.hpp"

void				Server::sendNumericReplies(const std::string &numericReplies, const std::string &replyString, Client &client)
{
	std::string returnMessage;

	returnMessage += this->prefix;
	returnMessage += " ";
	returnMessage += numericReplies;
	returnMessage += replyString;
	returnMessage += CR_LF;
	if (ERROR == send(client.getFd(), returnMessage.c_str(), returnMessage.length(), 0))
		std::cerr << ERROR_SEND_FAIL << std::endl;
}
void				Server::passHandler(const Message &message, Client &client)
{

	if (message.getParameters().empty())
		this->sendNumericReplies(ERR_NEEDMOREPARAMS, " :Not enough parameters", client);
	else if (client.getIsAuthorized())
		this->sendNumericReplies(ERR_ALREADYREGISTRED, " :You already reregistered", client);
	else if (this->pass == message.getParameter(0))
	{
		// TODO privmsg함수로 바꿔야함
		send(client.getFd(), "Password accepted\r\n", 20, 0);
		client.setIsAuthorized(true);
	}
}

/**
 * NICK 중복처리 케이스
 *
 * ERR_NICKCOLLISION이 발생하는 경우
 * => Ca의 NICK이 먼저 등록되어있고, 나중에 같은 NICK을 Cb가 등록하는 경우(다른 서버)
 * Sa - Sb - Cb(nick : dakim)
 * |
 * Ca(nick : dakim)
 *
 * ERR_NICKNAMEINUSE이 발생하는 경우
 * => Ca의 Nick이 먼저 등록되어있고,나중에 같은 NICK을 Cb가 등록하는 경우(같은 서버)
 * Sa - Cb(nick : dakim)
 * |
 * Ca(nick : dakim)
 *
 * 예외(무시함)
 * => Ca의 nick이 등록되어있고,나중에 같은 nick으로 재등록하려는 경우
 * Sa - Ca(nick : dakim)
 *
 **/

void				Server::nickHandler(const Message &message, Client &client)
{
	if (message.getParameters().empty())
		this->sendNumericReplies(ERR_NONICKNAMEGIVEN, " :No nickname given", client);
		/*
	else if (2 < message.getParameters().size())
		this->sendNumericReplies(ERR_NEEDMOREPARAMS, std::string(" ") + client.getServerName() + std::string(" NICK :Syntax error"), client);
		*/
	else if (!isValidNickname(message))
		this->sendNumericReplies(ERR_ERRONEUSNICKNAME, " :Erroneus nickname", client);
/*
	else if (this->acceptClients[message.getParameter(0)])
	{
		if ((!this->acceptClients[message.getParameter(0)].getHopCount() && 2 == message.getParameters().size())
		|| this->acceptClients[message.getParameter(0)].getHopCount() && 1 == messgae.getParameters().size())
		{
			// TODO 이후 kill커멘트만들고 수정필요
			this->sendNumericReplies(ERR_NICKCOLLISION, message.getParameter(0) + std::string(" :Nickname collision KILL"), client);
		}
		:dakim USER ~dakim localhost irc.example.net :123
		else if ((!this->acceptClients[message.getParameter(0)].getHopCount() && 1 == message.getParameter().size())
		|| this->acceptClients[message.getParameter(0)].getHopCount() && 2 == messgae.getParameter().size())
			this->sendNumericReplies(ERR_NICKNAMEINUSE, message.getParameter(0) + std::string(" :Nickname is already in use"), client);
	}
*/
	else
	{
		// 직접 연결된 클라이언트에서 온경우 + 중복되지 않은 nick을 보낸경우
		this->sendClients[message.getParameter(0)] = &client;
		if (client.getOriginNick() == "")
			client.setOriginNick(message.getParameter(0));
		client.setCurrentNick(message.getParameter(0));
	}
}

// void				Server::userHandler(const Message &message, Client &client)
// {


// }
