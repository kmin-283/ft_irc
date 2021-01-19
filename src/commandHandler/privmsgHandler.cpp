#include "Server.hpp"

int     Server::privmsgHandler(const Message &message, Client *client)
{
    client->setCurrentCommand("PRIVMSG");
    (void)message;
    printf("privmsg!!!!\n");
    return (CONNECT);
}