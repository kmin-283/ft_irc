//
// Created by kmin on 2021/02/12.
//
#include "Server.hpp"

void 	Server::sendChannelLists(Client *client)
{
	std::string 			prefix;
	std::string 			parameters;
	std::vector<Client *>	userList;
	std::vector<Client *>::iterator userIter;
	std::map<std::string, Client *>::iterator operIter;
	std::map<std::string, Channel>::iterator mapIter = this->localChannelList.begin();

	for (; mapIter != this->localChannelList.end(); ++mapIter)
	{
		std::cout << "111111111111!" << std::endl;
		if (mapIter->first[0] == '#')
		{
			std::map<std::string, Client *> &operList = mapIter->second.getOperators();
			userList = mapIter->second.getUsersList("all");
			userIter = userList.begin();
			for (; userIter != userList.end(); ++userIter) {
				this->sendMessage(Message(":" + (*userIter)->getInfo(NICK), "JOIN", mapIter->first), client);
			}
			operIter = operList.begin();
			prefix = ":" + operIter->second->getInfo(NICK);
			parameters = "";
			for (; operIter != operList.end(); ++operIter)
				parameters += operIter->second->getInfo(NICK) + " ";
			this->sendMessage(Message(prefix, "MODE", mapIter->first + " +o " + parameters), client);
			userList.clear();
		}
	}
	mapIter = this->remoteChannelList.begin();
	for (; mapIter != this->remoteChannelList.end(); ++mapIter)
	{
		std::cout << "22222222222222" << std::endl;
		std::map<std::string, Client *> &operList = mapIter->second.getOperators();
		userList = mapIter->second.getUsersList("all");
		userIter = userList.begin();
		for (; userIter != userList.end(); ++userIter) {
			this->sendMessage(Message(":" + (*userIter)->getInfo(NICK), "JOIN", mapIter->first), client);
		}
		operIter = operList.begin();
		prefix = ":" + operIter->second->getInfo(NICK);
		parameters = "";
		for (; operIter != operList.end(); ++operIter)
			parameters += operIter->second->getInfo(NICK) + " ";
		this->sendMessage(Message(prefix, "MODE", mapIter->first + " +o " + parameters), client);
		userList.clear();
	}
}
