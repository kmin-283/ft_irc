#include "Server.hpp"

int         Server::wallopsHandler(const Message&message, Client *client)
{
    if (client->getStatus() == USER)
    {

    }
    else // client == SERVER
    {
        std::cout << message.getTotalMessage();
    }
}