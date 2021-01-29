#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include "utils.hpp"

class       Client;

class		Channel
{
    private:
        std::string                         name;
        std::map<std::string, Client *>     joinedClients;
    
    public:
                                            Channel();
                                            Channel(const std::string &name);
                                            ~Channel();
        void                                setName(const std::string &name);
        const std::string                   getName(void) const;
        
        void                                joinChannel(Client *);
        void                                leaveChannel(Client *);
};

#endif
