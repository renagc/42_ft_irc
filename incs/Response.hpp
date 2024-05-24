#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include "Client.hpp"

# define RPL_WELCOME(nick, user) (":localhost 001 " + nick + " :Welcome to the ft_irc " + nick + "!" + user + "@localhost" + "\r\n")

// PING
# define RPL_PONG(nick, user, token) (":" + nick + "!" + user + "@localhost PONG " + token + "\r\n")
# define RPL_NOTICE(nick, user, target, message) (":" + nick + "!" + user + "@localhost NOTICE " + target + " " + message + "\r\n")

// NICK
# define ERR_NONICKNAMEGIVEN(nick) (":localhost 431 " + nick + " :No nickname given" + "\r\n");
# define ERR_ERRONEUSNICKNAME(nick, nickname) (":localhost 432 " + nick + " " + nickname + " :Erroneus nickname" + "\r\n");
# define ERR_NICKNAMEINUSE(nick, nickname) (":localhost 433 " + nick + " " + nickname + " :Nickname is already in use" + "\r\n");
# define RPL_NICK(nick, user, nickname) (":" + nick + "!" + user + "@localhost NICK " + nickname + "\r\n");

// USER
# define ERR_NEEDMOREPARAMS(nick, command) (":localhost 461 " + nick + " " + command + " :Not enough parameters" + "\r\n");

// JOIN
# define RPL_JOIN(nick, user, channel) (":" + nick + "!" + user + "@localhost JOIN #" + channel + "\r\n");

// PRIVMSG
# define ERR_NOSUCHNICK(nick, target) (":localhost 401 " + nick + " " + target + " :No such nick/channel" + "\r\n");
# define RPL_PRIVMSG(nick, user, target, message) (":" + nick + "!" + user + "@localhost PRIVMSG " + target + " :" + message + "\r\n");

// KICK
# define RPL_KICK(nick, user, channel, target) (":" + nick + "!" + user + "@localhost KICK #" + channel + " " + target + "\r\n");

#endif