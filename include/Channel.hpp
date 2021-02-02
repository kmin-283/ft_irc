#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include "utils.hpp"

class       Client;

class		Channel
{
    private:
        std::string                         name;
        std::map<std::string, Client *>     operators;      // 채널 관리자, 여러명 가능, 앞에 @ 붙음.
        std::map<std::string, Client *>     joinedUsers;
        std::string                         key;
        std::string                         topic;
    
    public:
                                            Channel();
                                            Channel(const std::string &name);
                                            ~Channel();
        void                                setName(const std::string &name);
        const std::string                   &getName(void) const;
        void                                setTopic(const std::string &topic);
        const std::string                   &getTopic(void) const;
        
        std::vector<Client *>               getUsersList(const std::string &mask);

        void                                enterUser(Client *user);
        void                                leaveUser(Client *user);

        std::string                         getUserNameList(void);

        // 임시로 만듬.
        int                                 getNumbersOfUsers(void);
        void                                showUsersName(void);

};

#endif
