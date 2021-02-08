#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include "utils.hpp"

# define MODE_UO 1
# define MODE_LO 2
# define MODE_V 4
# define MODE_A 8
# define MODE_LI 16
# define MODE_M 32
# define MODE_N 64
# define MODE_Q 128
# define MODE_P 256
# define MODE_S 512
# define MODE_R 1024
# define MODE_T 2048
# define MODE_K 4096
# define MODE_L 8192
# define MODE_B 16384
# define MODE_E 32768
# define MODE_UI 65536

typedef enum
{
    ONLY_LOCAL = 0,
    LOCAL_AND_REMOTE,
}       channelPrefix;

class       Client;

class		Channel
{
    public:
        typedef std::map<std::string, Client *>::iterator strClientIter;
    private:
        std::string                         name;
	std::map<std::string, Client *>     operators;      // 채널 관리자, 여러명 가능, 앞에 @ 붙음.
	std::map<std::string, Client *>     joinedUsers;
	std::string                         key;
	std::string                         topic;
	int                                 mode;
	int									maximumUser;
	std::set<std::string>				banList;
	std::set<std::string>				exceptionList;
	std::set<std::string>				invitationList;
	std::set<std::string>				ableToJoinList;
	//static std::set<std::string>		allUserList;
public:
	Channel();
	Channel(const std::string &name);
                                            ~Channel();
        void                                setName(const std::string &name);
        const std::string                   &getName(void) const;
        void                                setTopic(const std::string &topic);
        const std::string                   &getTopic(void) const;
        void 								setKey(const std::string &key);
        void 								clearKey(void);
        void 								setLimit(const std::string &limit);
        void 								clearLimit(void);
        void								setBanList(const std::string &banMask);
        void 								eraseBanList(const std::string &banMask);
        std::vector<std::string> 			*getBanList(void);
        void 								clearBanList(void);

		void								setExceptionList(const std::string &exceptionMask);
		void 								eraseExceptionList(const std::string &exceptionMask);
		std::vector<std::string> 			*getExceptionList(void);
		void 								clearExceptionList(void);

		void								setInvitationList(const std::string &invitataionMask);
		void 								eraseInvitationList(const std::string &invitationMask);
		std::vector<std::string> 			*getInvitationList(void);
		void 								clearInvitationList(void);

	void 								setMode(const int &mode, const bool &setConfig);
        const int 							&getMode(void) const;
        int 								toggleMode(std::string &successState, std::string &error, size_t &modeIndex, const Message &message);
        Client                              *findOperator(const std::string &userName);

        std::vector<Client *>               getUsersList(const std::string &mask);
        void                                enterUser(Client *user);
        void                                leaveUser(Client *user);

        std::string                         getUserNameList(void);
        // 임시로 만듬.
        int                                 getNumbersOfUsers(void);
        void                                showUsersName(void);
		void 								makeUserToOper(const std::string &operName);
        void 								makeOperToUser(const std::string &userName);
};

#endif
