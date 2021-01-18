#!/bin/sh
osascript -e 'tell app "Terminal"
    do script "cd ~/42/ft_irc \
./ircserv 3000 123 >> res/localhost.3000"
end tell'

osascript -e 'tell app "Terminal"
    do script "cd ~/42/ft_irc \
./ircserv 127.0.0.1:3000:123 3001 123 >> res/localhost.3001"
end tell'

osascript -e 'tell app "Terminal"
    do script "cd ~/42/ft_irc \
./ircserv 127.0.0.1:3000:123 3003 123 >> res/localhost.3003"
end tell'

osascript -e 'tell app "Terminal"
    do script "cd ~/42/ft_irc \
./ircserv 127.0.0.1:3001:123 3002 123 >> res/localhost.3002"
end tell'

sleep .1

osascript -e 'tell app "Terminal"
    do script "cd ~/42/ft_irc \
nc 127.0.0.1 3000 >> res/user-asd \
PASS 123 \
NICK asd \
USER asd asd asd asd \
VERSION \
VERSION localhost.3002 \
VERSION localhost.* \
VERSION * \
VERSION localhost.3??*" 
end tell'

sleep .1

osascript -e 'tell app "Terminal"
    do script "cd ~/42/ft_irc \
nc 127.0.0.1 3000 >> res/localhost.123 \
PASS 123 \
SERVER localhost.123 1 :123 \
:localhost.123 VERSION \
:localhost.123 VERSION localhost.3002 \
:localhost.123 VERSION localhost.* \
:localhost.123 VERSION * \
:localhost.123 VERSION localhost.3??*"
end tell'
