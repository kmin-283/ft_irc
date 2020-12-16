#include "Server.hpp"


void				Server::sendNumericReplies(const Message &message, Client &client)
{
	if (ERROR == send(client.getFd(), message.getTotalMessage().c_str(), message.getTotalMessage().length(), 0))
		std::cerr << ERROR_SEND_FAIL << std::endl;
}

void				Server::passHandler(const Message &message, Client &client)
{
	if (message.getParameters().empty())
		this->sendNumericReplies(Message(this->prefix, ERR_NEEDMOREPARAMS, " :Not enough parameters"), client);
	else if (client.getIsAuthorized())
		this->sendNumericReplies(Message(this->prefix, ERR_ALREADYREGISTRED, " :You already reregistered"), client);
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

/**
 * nick 메시지를 보내는 경우
 *
 * 1. 메시지 파리미터 체크 v
 * 2. nick의 형식에 맞지 않는경우 체크 v
 * 3. 클라이언트 위치 확인 (파라미터 개수 확인)
 * 	3-1. 직접 연결된 클라이언트(파라미터 1개)
 * 	=> NICK <nick>
 * 		3-1-1. PASS 등록 하지 않은경우
 * 		=> 연결 해제
 * 		3-1-2. PASS 등록한 경우
 * 			3-1-2-1. Nick 중복
 * 				3-1-2-1-1. 중복인 Nick에 해당하는 클라이언트가 같은 서버인 경우
 * 				=> NICK중복 에러출력
 * 				3-1-2-1-2. 중복인 Nick에 해당하는 클라이언트가 다른 서버에 연결되어 있는 경우
 * 				=> COLLISION 에러 발생
 * 				=> 직접 연결된 클라이언트 삭제 및 연결 해제
 * 				=> 연결된 다른 서버에 KILL, QUIT메시지 전송
 * 				3-1-2-1-3. 자기 자신의 기존의 Nick과 겹치는 경우
 * 				=> 무시
 * 			3-1-2-2. Nick 중복이 아닌경우
 * 				3-1-2-2-1. Nick을 처음 등록하는 경우
 * 				=> client의 originNick, currentNick에 저장
 * 				=> server의 sendClient map에 nick을 key로 Client*를 등록
 * 				3-1-2-2-2. Nick을 변경하는 경우
 * 				=> client의 currentNick 변경
 * 				=> server의 sendClient map에 기존에 등록되어있는 key를 삭제하고 새로운 Nick을 key로 등록
 * 	3-2. 직접 연결되지 않은 클라이언트(파라미터 2개) 중 처음으로 등록하는 경우 (두번째 파라미터 숫자 :1)
 * 	=> NICK <nick> :<hop-count>
 * 		3-2-1. Nick 중복
 * 		=> COLLISION 에러 발생
 * 		=> 직접 연결된 클라이언트 삭제 및 연결 해제
 * 		=> 연결된 다른 다른 서버에 KILL, QUIT메시지 전송
 * 		3-2-2. Nick 중복이 아닌경우
 * 		=> client 생성 및 Server의 클라이언트 관리하는 자료구조에 등록
 * 	3-3.직접 연결되지 않은 클라이언트(파라미터 2개)중 Nick을 변경하려는 경우 (두번째 파라미터 문자 :dakim)
 * 		=> NICK <nick> :<nick>
 * 		3-3-1. Nick 중복
 * 		=> NICK중복 에러출력
 * 		3-3-2. Nick 중복이 아닌경우
 * 		=> currentNick 변경
 * 		=> server에서 client관리하는 자료구조 갱신
*/

void				Server::nickHandler(const Message &message, Client &client)
{
	(void)message;
	(void)client;
	// if (message.getParameters().empty())
		// this->sendNumericReplies(Message(this->prefix, ERR_NONICKNAMEGIVEN, " :No nickname given"), client);
		/*
	else if (2 < message.getParameters().size())
		this->sendNumericReplies(ERR_NEEDMOREPARAMS, std::string(" ") + client.getServerName() + std::string(" NICK :Syntax error"), client);
		*/
	// this->sendNumericReplies(Message(message.getPrefix(), ERR_ALREADYREGISTRED, " :You already reregistered"), client);
	// else if (!isValidNickname(message))
		// this->sendNumericReplies(Message(this->prefix, ERR_ERRONEUSNICKNAME, " :Erroneus nickname"), client);
	// else if (!client.getIsAuthorized())
		// this->sendNumericReplies(Message(this->prefix, ERROR_STR, " :Bad Password"), client);
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
	// else
	// {
	// 	// TODO nick을 변경하려는 경우 sendClients에서 이전의 nick삭제 후 재등록 필요
	// 	// 직접 연결된 클라이언트에서 온경우 + 중복되지 않은 nick을 보낸경우
	// 	this->sendClients[message.getParameter(0)] = &client;
	// 	if (client.getOriginNick() == "")
	// 		client.setOriginNick(message.getParameter(0));
	// 	else
	// 		this->sendClients.erase(client.getCurrentNick());
	// 	client.setCurrentNick(message.getParameter(0));
	// 	if (client.isClientRegistered())
	// 		client.setStatus(USER);
	// 	if (client.getStatus() == USER)
	// 		std::cout << "client status = client" << std::endl;
	// }
}

void				Server::userHandler(const Message &message, Client &client)
{
	(void)message;
	(void)client;
	// std::string		userNick;

	// userNick = client.getCurrentNick() == "" ? "*" : client.getCurrentNick();
	// if (message.getParameters().size() != 4)
		// this->sendNumericReplies(Message(this->prefix, ERR_NEEDMOREPARAMS, std::string(" ") + userNick + std::string(" USER :Syntax error")), client);
	// else if ()
		// this->sendNumericReplies(ERR_ALREADYREGISTRED, std::string(" ") + userNick + std::string(" :Connection already registered"), client);
	// else if (!client.getIsAuthorized())
	// 	this->sendNumericReplies(Message(this->prefix, ERROR_STR, std::string(" ") + userNick + std::string(":Bad password")), client);
	// else
	// {
	// 	client.registerUser(message.getParameters());
	// 	if (client.isClientRegistered())
	// 		client.setStatus(USER);
	// 	if (client.getStatus() == USER)
	// 		std::cout << "client status = client" << std::endl;
	// }
}

// void				Server::serverHandler(const Message &message, Client &client)
// {

// }
