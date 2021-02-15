#include "Server.hpp"

std::vector<std::string>    getChannelNames(const std::string &name)
{
	std::vector<std::string>    names;
	std::stringstream           ss(name);
	std::string                 buffer;

	while (std::getline(ss, buffer, ','))
		names.push_back(buffer);
	return (names);
}

int     Server::joinHandler(const Message &message, Client *client)
{
	std::string                 check;
	std::string                 channelName;
	std::string                 fullChannelName;
	std::string 				modeParam;
	size_t 						modeParamIndex;
	std::vector<std::string>    channelNames;
	std::vector<Client *>       joinedUsers;
	Channel                     *targetChannel;
	Client						*targetUser;
	std::map<std::string, Channel>::iterator it;
	// 인자 애러 처리 해야함.

	client->setCurrentCommand("JOIN");
	if (client->getStatus() == UNKNOWN)
		return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
	else if (client->getStatus() == USER)
		this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
	else
		this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);

	if (client->getStatus() == USER)
	{
		channelNames = getChannelNames(message.getParameter(0));
		for (int i = 0; i < (int)channelNames.size(); i++)
		{
			fullChannelName = channelNames[i];
			// 이미 유저가 채널에 가입이 되어 있으면 아무일도 일어나지 않음
			if(client->findChannel(fullChannelName))
				continue;
			if(fullChannelName.at(0) == '#' || fullChannelName.at(0) == '&')
			{
				channelName = fullChannelName.substr(1);
				// 채널이 없으면 생성
				if ((it = this->localChannelList.find(fullChannelName)) != this->localChannelList.end())
					targetChannel = &it->second;
				else if((it = this->remoteChannelList.find(fullChannelName)) != this->remoteChannelList.end())
					targetChannel = &it->second;
				else
				{
					this->localChannelList[fullChannelName] = Channel(fullChannelName);
					targetChannel = &this->localChannelList[fullChannelName];
				}
				// 유저에 채널 리스트에도 채널 추가
				modeParamIndex = 1;
				targetUser = &this->sendClients[client->getInfo(NICK)];
				if (targetChannel->getMode() & MODE_K && !targetChannel->checkKey(message.getParameter(modeParamIndex)))
					return (this->*(this->replies[ERR_BADCHANNELKEY]))(message, targetUser);
				if (targetChannel->getMode() & MODE_LI && !targetUser->isInvited(fullChannelName))
					return (this->*(this->replies[ERR_INVITEONLYCAHN]))(message, targetUser);
				if (targetChannel->getMode() & MODE_B && targetChannel->isBanned(getClientPrefix(targetUser)))
				{
					if (!(targetChannel->getMode() & MODE_E) || !(targetChannel->isExcept(getClientPrefix(targetUser))))
						return (this->*(this->replies[ERR_BANNEDFROMCHAN]))(message, targetUser);
				}

				/*
				 * ngircd의 #1 채널에서 p*가 밴마스크로 등록되어있고
				 * 그 이후에 새롭게 3000번을 연결했을 때 3000번 서버 입장에서는
				 * #1이 밴 마스크가 있는지 없는지 알려주지 않음...
				 * 어떻게해야하는가?!
				 */
				client->joinChannel(targetChannel, fullChannelName);
				targetUser->delInviteChanList(fullChannelName);
				targetUser->joinChannel(targetChannel, fullChannelName);
				// 채널의 유저 리스트에 채널 추가
				targetChannel->enterUser(client);

				// 같은 채널 안에 있는 유저들에게 메시지 전송
				// 1. local channel user에게 메시지 전송
				// 2. broadcast 전송

				joinedUsers = targetChannel->getUsersList(this->serverName);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					if (joinedUsers[i]->getInfo(HOSTNAME) == this->serverName)
						this->sendMessage(Message(":" + getClientPrefix(client), "JOIN", ":" + fullChannelName), joinedUsers[i]);
				}
				// 353
				this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK)
																	  + " = "
																	  + fullChannelName
																	  + " :"
																	  + targetChannel->getUserNameList()), client);
				// 366
				this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK)
																		+ " "
																		+ fullChannelName
																		+ " :End of NAMES list"), client);
				// 다른 서버에 있는 유저들에게 join 는 알리는 메시지
				//this->broadcastMessage(Message(":" + getClientPrefix(client), "JOIN", ":" + fullChannelName), client);
				// 다른 서버에 있는 채널에 유저를 추가하는 메시지
				if (fullChannelName.at(0) == '#')
					this->broadcastMessage(Message(":" + client->getInfo(NICK), "JOIN", fullChannelName), client);
			}
				// 403 No such channel (채널 이름 오류)
			else
				this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + fullChannelName + " No such channel"), client);
			++modeParamIndex;
		}
		// UNKNOWN 일때 451 애러
	}
	else if (client->getStatus() == SERVER)
	{
		std::string     clientName;
		Client          *targetClient;
		// :kmin!2~@loa JOIN :#1
		clientName = message.getPrefix().substr(1);
		targetClient = &this->sendClients[clientName];
		// broadcast 메시지를 받음
		// local channel user를 봐서 있으면 메시지전송
		fullChannelName = message.getParameter(0);
		channelName = message.getParameter(0).substr(1);
		// 현재 서버에 같은 채널의 유저가 있는 경우
		if ((it = this->localChannelList.find(fullChannelName)) != this->localChannelList.end())
			targetChannel = &it->second;
		else if((it = this->remoteChannelList.find(fullChannelName)) != this->remoteChannelList.end())
			targetChannel = &it->second;
		else
		{
			this->remoteChannelList[fullChannelName] = Channel(fullChannelName);
			targetChannel = &this->remoteChannelList[fullChannelName];
		}
		targetClient->joinChannel(targetChannel, message.getParameter(0));
		joinedUsers = targetChannel->getUsersList(this->serverName);
		for (int i = 0; i < (int)joinedUsers.size(); i++)
			this->sendMessage(Message(getClientPrefix(joinedUsers[i])
				, "JOIN"
				, ":" + message.getParameter(0))
				, joinedUsers[i]);
		targetChannel->enterUser(targetClient);
		this->broadcastMessage(message, client);
	}
	return (CONNECT);
}

std::string getPartMessage(const Message& message)
{
	std::string returnMessage;

	if (message.getParameters().size() == 1)
		returnMessage = "";
	else if (message.getParameters().size() == 2)
	{
		if (message.getParameter(1).at(0) == ':')
			returnMessage = message.getParameter(1).substr(1);
		else
			returnMessage = message.getParameter(1);
	}
	else
	{
		returnMessage = message.getParameter(1).substr(1);
		for (int i = 2; i < (int)message.getParameters().size(); i++)
			returnMessage += message.getParameter(i);
	}
	return (returnMessage);
}

int     Server::partHandler(const Message &message, Client *client)
{
	std::string                 channelName;
	std::vector<std::string>    channelNames;
	std::vector<Client *>       joinedUsers;
	Channel                     *targetChannel = NULL;
	std::map<std::string, Channel>::iterator it;

	client->setCurrentCommand("PART");
	// ngircd는 이상하게 2개까지 받음 -> 2812가 2개까지 받음
	if (!(message.getParameters().size() == 1 ||
		  (message.getParameters().size() == 2) ||
		  (message.getParameters().size() > 2 && message.getParameter(1).at(0) == ':')))
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));

	if (client->getStatus() == USER)
	{
		channelNames = getChannelNames(message.getParameter(0));
		for (int i = 0; i < (int)channelNames.size(); i++)
		{
			channelName = channelNames[i];

			if (client->findChannel(channelName))
			{
				if ((it = this->localChannelList.find(channelName)) != this->localChannelList.end())
					targetChannel = &it->second;
				else if ((it = this->remoteChannelList.find(channelName)) != this->remoteChannelList.end())
					targetChannel = &it->second;

				client->leaveChannel(channelName);
				this->sendClients[client->getInfo(NICK)].leaveChannel(channelName);

				targetChannel->leaveUser(client);
				// 채널에 아무도 없으면 서버에서 채널 삭제
				if (targetChannel->getNumbersOfUsers() == 0)
				{
					if (this->localChannelList.find(channelName) != this->localChannelList.end())
						this->localChannelList.erase(channelName);
					else if (this->remoteChannelList.find(channelName) != this->remoteChannelList.end())
						this->remoteChannelList.erase(channelName);
					this->sendMessage(Message(":" + getClientPrefix(client), "PART", channelName + " :" + getPartMessage(message)), client);

					if (channelName.at(0) == '#')
						this->broadcastMessage(Message(":" + client->getInfo(NICK), "PART", channelName + " :" + getPartMessage(message)), client);
					// 채널에 혼자 남았을 때 자기 자신에게 part 메시지를 보내고 끝남.
					continue;
				}
				// 자신 한테도 part 메시지를 날려야 함.
				this->sendMessage(Message(":" + getClientPrefix(client), "PART", channelName + " :" + getPartMessage(message)), client);

				joinedUsers = targetChannel->getUsersList(this->serverName);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
					this->sendMessage(Message(":" + getClientPrefix(client), "PART", channelName + " :" + getPartMessage(message)), joinedUsers[i]);

				//:seunkim PART #m1 : 1 2 3 4 5
				if (channelName.at(0) == '#')
					this->broadcastMessage(Message(":" + client->getInfo(NICK), "PART", channelName + " :" + getPartMessage(message)), client);
			}
				// 442
			else if (this->localChannelList.find(channelName) != this->localChannelList.end())
				this->sendMessage(Message(this->prefix, ERR_NOTONCHANNEL, client->getInfo(NICK) + " " + channelName + " :You are not on that channel"), client);
				// 403
			else
				this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + channelName + " :No such channel"), client);
		}
	}
		// :seunkim PART #1 :123
	else if (client->getStatus() == SERVER)
	{
		std::string clientName;
		Client      *targetClient;

		clientName = message.getPrefix().substr(1);
		targetClient = &this->sendClients[clientName];

		channelName = message.getParameter(0);

		if ((it = this->localChannelList.find(channelName)) != this->localChannelList.end())
			targetChannel = &it->second;
		else if ((it = this->remoteChannelList.find(channelName)) != this->remoteChannelList.end())
			targetChannel = &it->second;

		targetClient->leaveChannel(channelName);
		targetChannel->leaveUser(targetClient);

		joinedUsers = targetChannel->getUsersList(this->serverName);
		for (int i = 0; i < (int)joinedUsers.size(); i++)
			this->sendMessage(Message(":" + getClientPrefix(targetClient), "PART", channelName + " :" + getPartMessage(message)), joinedUsers[i]);

		if (targetChannel->getNumbersOfUsers() == 0)
		{
			if (this->localChannelList.find(channelName) != this->localChannelList.end())
				this->localChannelList.erase(channelName);
			else if (this->remoteChannelList.find(channelName) != this->remoteChannelList.end())
				this->remoteChannelList.erase(channelName);
		}

		this->broadcastMessage(message, client);
	}
	//TODO:UNKNOWN 일때 451 애러
	// SERVER 일때 461???
	return (CONNECT);
}

std::string     getTopic(const Message &message)
{
	std::string returnTopic;

	// topic #my :
	// 콜론(:) 만 있고 뒤에 아무것도 없으면 다시 빈 topic 으로 됨.
	if (message.getParameter(1) == ":" && message.getParameters().size() == 2)
	{
		returnTopic = "";
		return (returnTopic);
	}
	if (message.getParameter(1).at(0) == ':')
	{
		returnTopic = message.getParameter(1).substr(1);
		for (int i = 2; i < (int)message.getParameters().size(); i++)
			returnTopic += (" " + message.getParameter(i));
	}
	else
		returnTopic = message.getParameter(1);
	return (returnTopic);
}

int     Server::topicHandler(const Message &message, Client *client)
{
	std::string             topic;
	std::string             channelName;
	Channel                 *targetChannel = NULL;
	std::map<std::string, Channel>::iterator it;
	std::vector<Client *>   joinedUsers;
	std::time_t             time;

	client->setCurrentCommand("TOPIC");
	// topic #my hello
	// topic #my :  hello 123 1 23
	if (!((message.getParameters().size() >= 1 && message.getParameters().size() <= 2)
		  || (message.getParameters().size() > 2 && message.getParameter(0).at(0) == ':')))
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));

	if (client->getStatus() == USER)
	{
		channelName = message.getParameter(0);
		// 서버리스트에 채널이 없을 떄
		if (this->localChannelList.find(channelName) == this->localChannelList.end()
			&& this->remoteChannelList.find(channelName) == this->remoteChannelList.end())
		{   // 403
			this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + channelName + " :No such channel"), client);
			return (CONNECT);
		}

		targetChannel = client->findChannel(channelName);
		// 유저가 채널에 들어가 있지 않을 때
		if (targetChannel == nullptr)
		{
			this->sendMessage(Message(this->prefix, ERR_NOTONCHANNEL, client->getInfo(NICK) + " " + channelName + " :You are not on that channel"), client);
			return(CONNECT);
		}

		// topic 조회 하기
		// topic #my
		if (message.getParameters().size() == 1)
		{
			if (targetChannel)
			{
				// topic이 없으면
				if (targetChannel->getTopic() == "")    // 331
					this->sendMessage(Message(this->prefix, RPL_NOTOPIC, client->getInfo(NICK) + " " + channelName + " :No topic is set"), client);
					// topic이 있으면
				else
				{
					// 332
					this->sendMessage(Message(this->prefix, RPL_TOPIC, client->getInfo(NICK) + " " + channelName + " :" + targetChannel->getTopic()), client);

					time = std::time(0);
					// 333
					this->sendMessage(Message(this->prefix, RPL_TOPICWHOTIME, client->getInfo(NICK) + " " + channelName + " " + client->getInfo(NICK) + " " + std::to_string(time)), client);
				}
			}
		}
		else if (message.getParameters().size() >= 2)
		{
			topic = getTopic(message);
			targetChannel->setTopic(topic);
			// 같은 채널에 있는 사람들에게 topic 메시지를 보냄(topic이 바뀌었다고 알림).
			joinedUsers = targetChannel->getUsersList(this->serverName);
			for (int i = 0; i < (int)joinedUsers.size(); i++)
				this->sendMessage(Message(getClientPrefix(client), "TOPIC", channelName + " :" + topic), joinedUsers[i]);
			// 다른 서버에도 보냄.
			if (message.getParameter(0).at(0) == '#')
				this->broadcastMessage(Message(":" + client->getInfo(NICK), "TOPIC", channelName + " :" + topic), client);
		}
	}
	else if (client->getStatus() == SERVER)
	{
		std::string    clientName;
		Client         *targetClient;

		// :song TOPIC #my :hello
		clientName = message.getPrefix().substr(1);
		std::cout << "clientName: " << clientName << std::endl;
		targetClient = &this->sendClients[clientName];
		std::cout << "targetClient: " << targetClient->getInfo(NICK) << std::endl;
		// broadcast 메시지를 받음
		// local channel user를 봐서 있으면 메시지전송
		channelName = message.getParameter(0);
		std::cout << "channelName = " << channelName << std::endl;
		// 현재 서버에 같은 채널의 유저가 있는 경우

		if ((it = this->localChannelList.find(channelName)) != this->localChannelList.end())
			targetChannel = &it->second;
		else if((it = this->remoteChannelList.find(channelName)) != this->remoteChannelList.end())
			targetChannel = &it->second;
		// else
		// {
		//     this->remoteChannelList[channelName] = Channel(channelName);
		//     targetChannel = &this->remoteChannelList[channelName];
		// }
		topic = getTopic(message);
		targetChannel->setTopic(topic);
		std::cout << "targetChannel = " << targetChannel->getName() << std::endl;
		std::cout << "this->serverName = " << this->serverName << std::endl;
		joinedUsers = targetChannel->getUsersList(this->serverName);
		for (int i = 0; i < (int)joinedUsers.size(); i++)
			this->sendMessage(Message(getClientPrefix(joinedUsers[i]), "TOPIC", message.getParameter(0) + " :" + topic), joinedUsers[i]);
		this->broadcastMessage(message, client);
	}
	return (CONNECT);
}

void 		Server::showExceptionList(Channel &channel, Client *client)
{
	std::set<std::string>::iterator it;
	std::set<std::string> 	&exceptList = channel.getExceptionList();

	it = exceptList.begin();
	for (; it != exceptList.end(); ++it)
	{
		this->sendMessage(Message(this->prefix
			, RPL_EXCEPTLIST
			, client->getInfo(NICK)
			  + " " + channel.getName()
			  + " " + *it
			  + " " + client->getInfo(NICK)
			  + std::to_string(std::time(NULL)))
			, client);
	}
	this->sendMessage(Message(this->prefix
		, RPL_ENDOFEXCEPTLIST
		, client->getInfo(NICK)
		  + " " + channel.getName()
		  + " :End of channel exception list")
		, client);
}
void 		Server::showInvitationList(Channel &channel, Client *client)
{
	std::set<std::string>::iterator it;
	std::set<std::string> &invitationList = channel.getInvitationList();

	it = invitationList.begin();
	for (; it != invitationList.end(); ++it)
	{
		this->sendMessage(Message(this->prefix
			, RPL_INVITELIST
			, client->getInfo(NICK)
			  + " " + channel.getName()
			  + " " + *it
			  + " " + client->getInfo(NICK)
			  + std::to_string(std::time(NULL)))
			, client);
	}
	this->sendMessage(Message(this->prefix
		, RPL_ENDOFINVITELIST
		, client->getInfo(NICK)
		  + " " + channel.getName()
		  + " :End of channel invitation list")
		, client);
}

void 		Server::showBanList(Channel &channel, Client *client)
{
	std::set<std::string>::iterator it;
	std::set<std::string> &banList = channel.getBanList();

	it = banList.begin();
	for (; it != banList.end(); ++it)
	{
		this->sendMessage(Message(this->prefix
			, RPL_BANLIST
			, client->getInfo(NICK)
			  + " " + channel.getName()
			  + " " + *it
			  + " " + client->getInfo(NICK)
			  + std::to_string(std::time(NULL)))
			, client);
	}
	this->sendMessage(Message(this->prefix
		, RPL_ENDOFBANLIST
		, client->getInfo(NICK)
		  + " " + channel.getName()
		  + " :End of channel ban list")
		, client);
}

// mode + channel + mode + modeparameter
// 만약 mode만 주어지고 mode parameter가 주어지지않는 경우에는 해당 모드를 가진 list를 출력함

static void setFullMask(std::string &fullMask, const std::string &mask)
{
	std::string defaultString = "*!*@*";

	if (mask.find('!') == std::string::npos)
		fullMask = mask + defaultString.substr(1);
	else if (mask.find('@') == std::string::npos)
		fullMask = mask + defaultString.substr(3);
	else
		fullMask = mask;
}

int			Server::modeHelper(std::string &error, size_t &modeIndex, const Message &message, Client *client, Client *exceptClient)
{
	bool        isAdd = true;
	char        mode;
	char 		sign;
	size_t      modeParamIndex;
	size_t      modeCharIndex = 0;
	std::string	param;
	std::map<std::string, Client>::iterator		clientIter;
	std::map<std::string, Channel>::iterator 	chanIter;
	std::vector<Client *>						joinedUsers;

	sign = '+';
	if (message.getParameter(modeIndex)[0] == '-')
	{
		isAdd = false;
		sign = '-';
	}
	if (message.getParameter(modeIndex)[0] == '-' || message.getParameter(modeIndex)[0] == '+')
		modeCharIndex += 1;
	mode = message.getParameter(modeIndex)[modeCharIndex];
	modeParamIndex = modeIndex + 1;
	while (mode != 0)
	{
		switch (mode) {
			case 'o':
				if (modeParamIndex >= message.getParameters().size())
				{
					error = ERR_NEEDMOREPARAMS;
					return (-1); //need more params
				}
				clientIter = this->sendClients.find(message.getParameter(modeParamIndex));
				if (clientIter == this->sendClients.end())
				{
					error = ERR_NOSUCHNICK;
					return (-1);
				}
				else if (!clientIter->second.findChannel(message.getParameter(0)))
				{
					error = ERR_USERNOTINCHANNEL;
					return (-1);
				}
				chanIter = this->localChannelList.find(message.getParameter(0));
				if (chanIter == this->localChannelList.end())
					chanIter = this->remoteChannelList.find(message.getParameter(0));
				if (isAdd)
					chanIter->second.makeUserToOper(message.getParameter(modeParamIndex));
				else
					chanIter->second.makeOperToUser(message.getParameter(modeParamIndex));
				joinedUsers = chanIter->second.getUsersList(this->serverName);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					this->sendMessage(Message(":" + getClientPrefix(client)
						, "MODE"
						, chanIter->second.getName()
						  + " " + sign + "o"
						  + " " + message.getParameter(modeParamIndex)), joinedUsers[i]);
				}
				broadcastMessage(Message(":" + exceptClient->getInfo(NICK)
					, "MODE"
					, 	 message.getParameter(0)
						  + " " + message.getParameter(1)
						  + " " + message.getParameter(2))
					, exceptClient);
				++modeParamIndex;
				break;

			case 'v':
				if (modeParamIndex >= message.getParameters().size())
				{
					error = ERR_NEEDMOREPARAMS;
					return (-1); //need more params
				}
				clientIter = this->sendClients.find(message.getParameter(modeParamIndex));
				if (clientIter == this->sendClients.end())
				{
					error = ERR_NOSUCHNICK;
					return (-1);
				}
				else if (!clientIter->second.findChannel(message.getParameter(0)))
				{
					error = ERR_USERNOTINCHANNEL;
					return (-1);
				}
				joinedUsers = chanIter->second.getUsersList(this->serverName);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					this->sendMessage(Message(":" + getClientPrefix(client)
						, "MODE"
						, chanIter->second.getName()
						  + " " + sign + "v"
						  + " " + message.getParameter(modeParamIndex)), joinedUsers[i]);
				}
				broadcastMessage(Message(":" + exceptClient->getInfo(NICK)
					, "MODE"
					, 	 message.getParameter(0)
						  + " " + message.getParameter(1)
						  + " " + message.getParameter(2))
					, exceptClient);
				++modeParamIndex;
				break;

			case 'k':
				if (isAdd && modeParamIndex >= message.getParameters().size())
				{
					error = ERR_NEEDMOREPARAMS;
					return (-1); //need more params
				}
				chanIter = this->localChannelList.find(message.getParameter(0));
				if (chanIter == this->localChannelList.end())
					chanIter = this->remoteChannelList.find(message.getParameter(0));
				if (isAdd)
					chanIter->second.setKey(message.getParameter(modeParamIndex));
				else
					chanIter->second.clearKey();
				joinedUsers = chanIter->second.getUsersList(this->serverName);
				if (modeParamIndex >= message.getParameters().size())
					param = "*";
				else
					param = message.getParameter(modeParamIndex);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					this->sendMessage(Message(":" + getClientPrefix(client)
						, "MODE"
						, chanIter->second.getName()
						  + " " + sign + "k"
						  + " " + param), joinedUsers[i]);
				}
				broadcastMessage(Message(":" + exceptClient->getInfo(NICK)
					, "MODE"
					, 	 message.getParameter(0)
						  + " " + message.getParameter(1)
						  + " " + message.getParameter(2))
					, exceptClient);
				++modeParamIndex;
				break;

			case 'l':
				if (isAdd && modeParamIndex >= message.getParameters().size())
				{
					error = ERR_NEEDMOREPARAMS;
					return (-1); //need more params
				}
				chanIter = this->localChannelList.find(message.getParameter(0));
				if (chanIter == this->localChannelList.end())
					chanIter = this->remoteChannelList.find(message.getParameter(0));
				if (isAdd)
					chanIter->second.setLimit(message.getParameter(modeParamIndex));
				else
					chanIter->second.clearLimit();
				joinedUsers = chanIter->second.getUsersList(this->serverName);
				if (modeParamIndex >= message.getParameters().size())
					param = "*";
				else
					param = message.getParameter(modeParamIndex);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					this->sendMessage(Message(":" + getClientPrefix(client)
						, "MODE"
						, chanIter->second.getName()
						  + " " + sign + "l"
						  + " " + param), joinedUsers[i]);
				}
				broadcastMessage(Message(":" + exceptClient->getInfo(NICK)
					, "MODE"
					, 	 message.getParameter(0)
						  + " " + message.getParameter(1)
						  + " " + message.getParameter(2))
					, exceptClient);
				++modeParamIndex;
				break;

			case 'b':
				chanIter = this->localChannelList.find(message.getParameter(0));
				if (chanIter == this->localChannelList.end())
					chanIter = this->remoteChannelList.find(message.getParameter(0));
				if (isAdd && exceptClient->getStatus() == USER && modeParamIndex >= message.getParameters().size())
				{
					this->showBanList(chanIter->second, client);
					break;
				}
				if (modeParamIndex >= message.getParameters().size())
					param = "*";
				else
					setFullMask(param, message.getParameter(modeParamIndex));
				if (isAdd)
					chanIter->second.setList(chanIter->second.getBanList(), MODE_B, param);
				else if (!isAdd && modeParamIndex >= message.getParameters().size())
					chanIter->second.clearList(chanIter->second.getBanList(), MODE_B);
				else
					chanIter->second.eraseElement(chanIter->second.getBanList(), MODE_B, param);
				joinedUsers = chanIter->second.getUsersList(this->serverName);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					this->sendMessage(Message(":" + getClientPrefix(client)
						, "MODE"
						, chanIter->second.getName()
						  + " " + sign + "b"
						  + " " + param), joinedUsers[i]);
				}
				broadcastMessage(Message(":" + exceptClient->getInfo(NICK)
					, "MODE"
					, 	 message.getParameter(0)
						  + " " + message.getParameter(1)
						  + " " + param)
					, exceptClient);
				++modeParamIndex;
				break;

			case 'e':
				chanIter = this->localChannelList.find(message.getParameter(0));
				if (chanIter == this->localChannelList.end())
					chanIter = this->remoteChannelList.find(message.getParameter(0));
				if (isAdd && exceptClient->getStatus() == USER && modeParamIndex >= message.getParameters().size())
				{
					this->showExceptionList(chanIter->second, client);
					break;
				}
				if (modeParamIndex >= message.getParameters().size())
					param = "*";
				else
					setFullMask(param, message.getParameter(modeParamIndex));
				if (isAdd)
					chanIter->second.setList(chanIter->second.getExceptionList(), MODE_E, param);
				else if (!isAdd && modeParamIndex >= message.getParameters().size())
					chanIter->second.clearList(chanIter->second.getExceptionList(), MODE_E);
				else
					chanIter->second.eraseElement(chanIter->second.getExceptionList(), MODE_E, param);
				joinedUsers = chanIter->second.getUsersList(this->serverName);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					this->sendMessage(Message(":" + getClientPrefix(client)
						, "MODE"
						, chanIter->second.getName()
						  + " " + sign + "e"
						  + " " + param), joinedUsers[i]);
				}
				broadcastMessage(Message(":" + exceptClient->getInfo(NICK)
					, "MODE"
					, 	 message.getParameter(0)
						  + " " + message.getParameter(1)
						  + " " + param)
					, exceptClient);
				++modeParamIndex;
				break;

			case 'I':
				chanIter = this->localChannelList.find(message.getParameter(0));
				if (chanIter == this->localChannelList.end())
					chanIter = this->remoteChannelList.find(message.getParameter(0));
				if (isAdd && exceptClient->getStatus() == USER && modeParamIndex >= message.getParameters().size())
				{
					this->showInvitationList(chanIter->second, client);
					break;
				}
				if (modeParamIndex >= message.getParameters().size())
					param = "*";
				else
					setFullMask(param, message.getParameter(modeParamIndex));
				if (isAdd)
					chanIter->second.setList(chanIter->second.getInvitationList(), MODE_UI, param);
				else if (!isAdd && modeParamIndex >= message.getParameters().size())
					chanIter->second.clearList(chanIter->second.getInvitationList(), MODE_UI);
				else
					chanIter->second.eraseElement(chanIter->second.getInvitationList(), MODE_UI, param);
				joinedUsers = chanIter->second.getUsersList(this->serverName);
				for (int i = 0; i < (int)joinedUsers.size(); i++)
				{
					this->sendMessage(Message(":" + getClientPrefix(client)
						, "MODE"
						, chanIter->second.getName()
						  + " " + sign + "I"
						  + " " + param), joinedUsers[i]);
				}
				broadcastMessage(Message(":" + exceptClient->getInfo(NICK)
					, "MODE"
					, 	 message.getParameter(0)
						  + " " + message.getParameter(1)
						  + " " + param)
					, exceptClient);
				++modeParamIndex;
				break;

			default:
				break;
		}
		mode = message.getParameter(modeIndex)[++modeCharIndex];
	}
	return (modeParamIndex + 1);
}

int         Server::modeHandler(const Message &message, Client *client)
{
	std::string check;
	size_t parameterSize;
	std::map<std::string, Channel>::iterator targetChannel;
	std::vector<Client *>						joinedUsers;
	Client *targetClient;

	client->setCurrentCommand("MODE");
	if (client->getStatus() == UNKNOWN)
		return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
	else if (client->getStatus() == USER)
		this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
	else
		this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);

	parameterSize = message.getParameters().size();
	if (parameterSize < 2)
		return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
	targetChannel = this->localChannelList.find(message.getParameter(0));
	if (targetChannel == this->localChannelList.end())
		targetChannel = this->remoteChannelList.find(message.getParameter(0));
	if (targetChannel == this->remoteChannelList.end())
	{
		this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + message.getParameter(0) + " :No such channel"), client);
		return (CONNECT);
	}

	// ERR_NOSUCHCHANNEL 이후 return
	if (client->getStatus() == USER &&
		(targetClient = targetChannel->second.findOperator(client->getInfo(NICK))) == NULL)
		return (CONNECT); // return --> 482 you are not channel operator
	if (client->getStatus() == SERVER &&
		(targetClient = targetChannel->second.findOperator(message.getPrefix().substr(1))) == NULL)
		return (CONNECT); // return --> error
	size_t modeIndex = 1;
	size_t modeParamIndex;
	std::string error = "";
	std::string successState = "";
	while (modeIndex < message.getParameters().size()) // 어떤 조건문
	{
		if ((modeParamIndex = this->modeHelper(error, modeIndex, message, targetClient, client)) == ERROR)
			return (this->*(this->replies[error]))(message, client);
		error.clear();
		if (targetChannel->second.toggleMode(successState, error, modeIndex, message) == ERROR)
			return (this->*(this->replies[error]))(message, client);
		modeIndex += modeParamIndex;
	}
	if (successState.size() > 1)
	{
		joinedUsers = targetChannel->second.getUsersList(this->serverName);
		for (int i = 0; i < (int) joinedUsers.size(); i++) {
			this->sendMessage(Message(":" + getClientPrefix(targetClient), "MODE",
									  targetChannel->second.getName() + " " + successState + " " +
									  targetClient->getInfo(NICK)), joinedUsers[i]);
		}
		broadcastMessage(message, client);
	}
	return (CONNECT);
}

int     Server::namesHandler(const Message &message, Client *client)
{
<<<<<<< HEAD
	std::string                                 noChannelUserList;
	std::map<std::string, Channel>::iterator    it;
	std::vector<std::string>                    channelNames;

	client->setCurrentCommand("NAMES");
	if (!(message.getParameters().size() >= 0 && message.getParameters().size() <= 2))
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));

	if (message.getParameters().size() == 0)
	{
		it = this->localChannelList.begin();
		for (; it != this->localChannelList.end(); it++)
			this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
		it = this->remoteChannelList.begin();
		for (; it != this->remoteChannelList.end(); it++)
			this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
		// 아무 채널에도 속해 있지 않는 유저들 찾기
		strClientPtrIter pit = this->clientList.begin();
		for (; pit != this->clientList.end(); pit++)
		{
			// TODO: clientList랑 client랑 채널 정보가 다름
			if (pit->second->getNumbersOfJoinedChannels() == 0)
				noChannelUserList += (pit->first + " ");
		}
		if (!noChannelUserList.empty())
			this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " * * :" + noChannelUserList), client);
		// 366
		this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK) + " * :End of NAMES list"), client);
	}
	else if (message.getParameters().size() == 1)
	{
		channelNames = getChannelNames(message.getParameter(0));
		for (int i = 0; i < (int)channelNames.size(); i++)
		{
			if ((it = this->localChannelList.find(channelNames[i])) != this->localChannelList.end())
				this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
			else if ((it = this->remoteChannelList.find(channelNames[i])) != this->remoteChannelList.end())
				this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
			this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK) + " " + channelNames[i] + " :End of NAMES list"), client);
		}
	}
	else
	{

	}
	return (CONNECT);
=======
    std::string                                 noChannelUserList;
    std::map<std::string, Channel>::iterator    it;
    std::vector<std::string>                    channelNames;

    client->setCurrentCommand("NAMES");
    if (!(message.getParameters().size() >= 0 && message.getParameters().size() <= 2))
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    
    if (message.getParameters().size() == 0)
    {
        it = this->localChannelList.begin();
        for (; it != this->localChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
        it = this->remoteChannelList.begin();
        for (; it != this->remoteChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
        // 아무 채널에도 속해 있지 않는 유저들 찾기
        strClientPtrIter pit = this->clientList.begin();
        for (; pit != this->clientList.end(); pit++)
        {   
            // 연결된 다른 서버의 유저들도 출력이 되어야 함.
            // TODO: clientList-> sendClients로 바꾸어야 함.
            if (pit->second->getNumbersOfJoinedChannels() == 0) 
                noChannelUserList += (pit->first + " ");
        }
        if (!noChannelUserList.empty())
            this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " * * :" + noChannelUserList), client);
        // 366
        this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK) + " * :End of NAMES list"), client);
    }
    else if (message.getParameters().size() == 1)
    {
        channelNames = getChannelNames(message.getParameter(0));
        for (int i = 0; i < (int)channelNames.size(); i++)
        {
            if ((it = this->localChannelList.find(channelNames[i])) != this->localChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
            else if ((it = this->remoteChannelList.find(channelNames[i])) != this->remoteChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
            this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK) + " " + channelNames[i] + " :End of NAMES list"), client);            
        }
    }
    else
    {

    }
    return (CONNECT);
>>>>>>> ac5f76677edd2b9e6abb0923e9952741aecdb1c4
}

int     Server::listHandler(const Message &message, Client *client)
{
<<<<<<< HEAD
	std::map<std::string, Channel>::iterator it;
	std::vector<std::string> channelNames;

	client->setCurrentCommand("LIST");
	if (!(message.getParameters().size() >= 0 && message.getParameters().size() <= 2))
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));

	// 321
	this->sendMessage(Message(this->prefix, RPL_LISTSTART, client->getInfo(NICK) + " Channel :Users  Names"), client);

	if (message.getParameters().size() == 0)
	{
		it = this->localChannelList.begin();
		for (; it != this->localChannelList.end(); it++)
			this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
		it = this->remoteChannelList.begin();
		for (; it != this->remoteChannelList.end(); it++)
			this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
		this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " &SERVER 0 :Server Messages"), client);
	}
	else if (message.getParameters().size() == 1)
	{
		channelNames = getChannelNames(message.getParameter(0));
		for (int i = 0; i < (int)channelNames.size(); i++)
		{
			if ((it = this->localChannelList.find(channelNames[i])) != this->localChannelList.end())
				this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
			else if ((it = this->remoteChannelList.find(channelNames[i])) != this->remoteChannelList.end())
				this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
		}
	}

	// 323
	this->sendMessage(Message(this->prefix, RPL_LISTEND, client->getInfo(NICK) + " :End of LIST"), client);
	return (CONNECT);
=======
    std::map<std::string, Channel>::iterator it;
    std::vector<std::string> channelNames;

    client->setCurrentCommand("LIST");
    if (!(message.getParameters().size() >= 0 && message.getParameters().size() <= 2))
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));

    // 321
    this->sendMessage(Message(this->prefix, RPL_LISTSTART, client->getInfo(NICK) + " Channel :Users  Names"), client);

    if (message.getParameters().size() == 0)
    {
        it = this->localChannelList.begin();
        for (; it != this->localChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
        it = this->remoteChannelList.begin();
        for (; it != this->remoteChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
        this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " &SERVER 0 :Server Messages"), client);
    }
    else if (message.getParameters().size() == 1)
    {
        channelNames = getChannelNames(message.getParameter(0));
        for (int i = 0; i < (int)channelNames.size(); i++)
        {
            if ((it = this->localChannelList.find(channelNames[i])) != this->localChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
            else if ((it = this->remoteChannelList.find(channelNames[i])) != this->remoteChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
        }
    }

    // 323
    this->sendMessage(Message(this->prefix, RPL_LISTEND, client->getInfo(NICK) + " :End of LIST"), client);
    return (CONNECT);
}

int     Server::inviteHandler(const Message &message, Client *client)
{
    Client      *targetUser;
    Channel     *targetChannel;
    std::string prefix;
    
    if (message.getParameters().size() != 2)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);

    // INVITE seunkim #my

    // ngircd는 채널 확인을 안함.
    // // 서버의 채널이 있는지 확인
    // if (client->getStatus() == USER)
    // {
    // 유저가 있는지 확인
    if (this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
        targetUser = &this->sendClients[message.getParameter(0)];
    else
        return (this->*(this->replies[ERR_NOSUCHNICK]))(message, client);   // 401
    // 채널이 서버에 있는지 확인
    if (this->localChannelList.find(message.getParameter(1)) != this->localChannelList.end())
        targetChannel = &this->localChannelList[message.getParameter(1)];
    else if (this->remoteChannelList.find(message.getParameter(1)) != this-> remoteChannelList.end())
        targetChannel = &this->remoteChannelList[message.getParameter(1)];
    else
    {   // 403
        this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + message.getParameter(1) + " :No such channel"), client);
        return (CONNECT);
    }
    // 유저가 채널에 들어가 있는지 확인
    if (client->getStatus() == USER && !client->findChannel(message.getParameter(1)))
    {   // 442
        this->sendMessage(Message(this->prefix, ERR_NOTONCHANNEL, client->getInfo(NICK) + " " + message.getParameter(1) + " :You are not on that channel"), client);
        return (CONNECT);
    }
    // 채널에 관리자인지 확인하기
    if (client->getStatus() == USER && !targetChannel->findOperator(client->getInfo(NICK)))
    {   // 482
        this->sendMessage(Message(this->prefix, ERR_CHANOPRIVSNEEDED, client->getInfo(NICK) + " " + message.getParameter(1) + " :You're not channel operator"), client);
        return (CONNECT);
    }
    // 채널에 유저가 이미 있는지 확인
    if (targetChannel->findJoinedUser(message.getParameter(0)))
    {   // 443
        this->sendMessage(Message(this->prefix, ERR_USERONCHANNEL, message.getParameter(0) + " " + message.getParameter(1) + " :is already on channel"), client);
        return (CONNECT);
    }        
    // 같은 서버에 유저가 있다면 메세지를 보냄
    if (this->clientList.find(message.getParameter(0)) != this->clientList.end())
    {   
        //TODO: targetChannel이 i 모드 일떄만 추가해야 하는가.
        targetUser->addInvitedChannel(message.getParameter(1));
        prefix = getClientPrefix(&this->sendClients[message.getPrefix().substr(1)]);
        this->sendMessage(Message(":" + prefix, "INVITE", targetUser->getInfo(NICK) + " " + message.getParameter(1)), targetUser);
        prefix = getClientPrefix(targetUser);
        this->sendMessage(Message(":" + prefix, RPL_INVITING, message.getPrefix().substr(1) + " " + message.getParameter(0) + " " + message.getParameter(1)), &this->sendClients[message.getPrefix().substr(1)]);
    }
    // 아니면 다른서버에 메시지 보냄.
    else
    {
        if (message.getPrefix().empty())
            prefix = client->getInfo(NICK);
        else
            prefix = message.getPrefix().substr(1);
        this->sendMessage(Message(":" + prefix, "INVITE", message.getParameter(0) +  " " + message.getParameter(1)), targetUser);
    }
    return (CONNECT);
}

int         Server::kickHandler(const Message &message, Client *client)
{
    Channel                 *targetChannel;
    Client                  *targetUser;
    std::string             prefix;
    std::string             kickMessage;
    std::vector<Client *>   joinedUsers;

    if (!(message.getParameters().size() >= 2 && message.getParameters().size() <= 3))
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);

    // KICK #my kmin hello

    // 403 ERR_NOSUCHCHANNEL "<channel name> :No such channel"
    if (this->localChannelList.find(message.getParameter(0)) != this->localChannelList.end())
        targetChannel = &this->localChannelList[message.getParameter(0)];
    else if (this->remoteChannelList.find(message.getParameter(0)) != this->remoteChannelList.end())
        targetChannel = &this->remoteChannelList[message.getParameter(0)];
    else
    {
        this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + message.getParameter(0) + " :No such channel"), client);
        return (CONNECT);
    }
    // 401 ERR_NOSUCHNICK "<nickname> :No such nick/channel"
    if (this->sendClients.find(message.getParameter(1)) != this->sendClients.end())
        targetUser = &this->sendClients[message.getParameter(1)];
    else
    {
        this->sendMessage(Message(this->prefix, ERR_NOSUCHNICK, client->getInfo(NICK) + " " + message.getParameter(1) + " :No such nick or channel name"), client);
        return (CONNECT);
    }
    // 442 ERR_NOTONCHANNEL "<channel> :You're not on that channel"
    if (client->getStatus() == USER && !(targetChannel->findJoinedUser(client->getInfo(NICK)) || targetChannel->findOperator(client->getInfo(NICK))))
    {
        this->sendMessage(Message(this->prefix, ERR_NOTONCHANNEL, client->getInfo(NICK) + " " + message.getParameter(0) + " :You are not on that channel"), client);
        return (CONNECT);
    }
    // 482 ERR_CHANOPRIVSNEEDED "<channel> :You're not channel operator"
    if (client->getStatus() == USER && !targetChannel->findOperator(client->getInfo(NICK)))
    {
        this->sendMessage(Message(this->prefix, ERR_CHANOPRIVSNEEDED, client->getInfo(NICK) + " " + message.getParameter(0) + " :You're not channel operator"), client);
        return (CONNECT);
    }
    // 441 ERR_USERNOTINCHANNEL "<nick> <channel> :They aren't on that channel"
    if (!(targetChannel->findOperator(message.getParameter(1)) || targetChannel->findJoinedUser(message.getParameter(1))))
    {
        this->sendMessage(Message(this->prefix, ERR_USERNOTINCHANNEL, message.getParameter(1) + " " + message.getParameter(0) + " :They aren't on that channel"), client);
        return (CONNECT);
    }
    // 같은 서버에 유저가 있을 때
    
    if (message.getPrefix().empty())
        prefix = client->getInfo(NICK);
    else
        prefix = message.getPrefix().substr(1);
    if (message.getParameters().size() > 2)
        kickMessage = message.getParameter(2);
    else
        kickMessage = ":" + prefix;

    joinedUsers = targetChannel->getUsersList(this->serverName);
    for (int i = 0; i < (int)joinedUsers.size(); i++)
        this->sendMessage(Message(getClientPrefix(&this->sendClients[prefix])
                                    , "KICK"
                                    , message.getParameter(0)
                                    + " " + message.getParameter(1)
                                    + " " + kickMessage)
                                    , joinedUsers[i]);

    targetChannel->leaveUser(targetUser);
    targetUser->leaveChannel(message.getParameter(0));

    this->broadcastMessage(Message(":" + prefix
                                    , "KICK"
                                    , message.getParameter(0)
                                    + " " + message.getParameter(1)
                                    + " " + kickMessage), client);
    
    // :seunkim KICK #My kmin :seunkim
    // :seunkim!~q@localhost KICK #my kmin :seunkim
    return (CONNECT);
>>>>>>> ac5f76677edd2b9e6abb0923e9952741aecdb1c4
}