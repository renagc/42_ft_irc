#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include "Client.hpp"

# define USER(nick, user, host) (nick + "!~" + user + "@" + host)
# define RPL_WELCOME(nick, user, host) (":localhost 001 " + nick + " :Welcome to the ft_irc " + USER(nick, user, host) + "\r\n")

// PING
# define RPL_PONG(nick, user, token) (":" + nick + "!" + user + "@localhost PONG " + token + "\r\n")
# define RPL_NOTICE(nick, user, target, message) (":" + nick + "!" + user + "@localhost NOTICE " + target + " " + message + "\r\n")

// NICK
# define ERR_NONICKNAMEGIVEN(nick) (":localhost 431 " + nick + " :No nickname given" + "\r\n");
# define ERR_ERRONEUSNICKNAME(nick, nickname) (":localhost 432 " + nick + " " + nickname + " :Erroneus nickname" + "\r\n");
# define ERR_NICKNAMEINUSE(nick, nickname) (":localhost 433 " + nick + " " + nickname + " :Nickname is already in use" + "\r\n");
# define RPL_NICK(nick, user, host, nickname) (":" + USER(nick, user, host) + " NICK :" + nickname + "\r\n");

// USER
# define ERR_NEEDMOREPARAMS(nick, command) (":localhost 461 " + nick + " " + command + " :Not enough parameters" + "\r\n");

// JOIN
# define RPL_JOIN(nick, user, host, channel) (":" + USER(nick, user, host) + " JOIN #" + channel + "\r\n");

// PRIVMSG
# define ERR_NOSUCHNICK(nick, target) (":localhost 401 " + nick + " " + target + " :No such nick/channel" + "\r\n");
# define RPL_PRIVMSG(nick, user, target, message) (":" + nick + "!" + user + "@localhost PRIVMSG " + target + " :" + message + "\r\n");

// KICK
# define RPL_KICK(nick, user, channel, target) (":" + nick + "!" + user + "@localhost KICK #" + channel + " " + target + "\r\n");

// Invalid command
# define ERR_UNKNOWNCOMMAND(nick, command) (":localhost 421 " + nick + " " + command + " :Unknown command" + "\r\n");

// CHANNEL MESSAGE
# define RPL_CHANNEL(nick, user, host, channel, message) (":" + USER(nick, user, host) + " PRIVMSG #" + channel + " :" + message + "\r\n");

#endif