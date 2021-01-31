#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include "utils.hpp"

class       Client;

class		Channel
{
    private:
        std::string                         name;
        std::map<std::string, Client *>     joinedUsers;
        std::string                         key;
    
    public:
                                            Channel();
                                            Channel(const std::string &name);
                                            ~Channel();
        void                                setName(const std::string &name);
        const std::string                   getName(void) const;
        
        std::vector<Client *>               getUsersList(void);

        void                                enterUser(Client *user);
        void                                leaveUser(Client *user);

        std::string                         getUserNameList(void);

        // 임시로 만듬.
        int                                 getSize(void);
        void                                showUsersName(void);
};

#endif
