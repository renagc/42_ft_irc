#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <sys/socket.h>
# include <vector>
# include <string>
# include <map>

// irc libs
# include "Channel.hpp"
# include "Client.hpp"
# include "Server.hpp"

# define USER(nick, user, host) (nick + "!~" + user + "@" + host)

// PING
# define RPL_PONG(nick, user, token) (":" + nick + "!" + user + "@localhost PONG " + token + "\r\n")
# define RPL_NOTICE(nick, user, target, message) (":" + nick + "!" + user + "@localhost NOTICE " + target + " " + message + "\r\n")

// PRIVMSG
# define ERR_NOSUCHNICK(nick, target) (":localhost 401 " + nick + " " + target + " :No such nick/channel" + "\r\n");
# define RPL_PRIVMSG(nick, user, target, message) (":" + nick + "!" + user + "@localhost PRIVMSG " + target + " :" + message + "\r\n");

// KICK
# define RPL_KICK(nick, user, channel, target) (":" + nick + "!" + user + "@localhost KICK #" + channel + " " + target + "\r\n");

// Invalid command
# define ERR_UNKNOWNCOMMAND(nick, command) (":localhost 421 " + nick + " " + command + " :Unknown command" + "\r\n");

// CHANNEL MESSAGE
# define RPL_CHANNEL(nick, user, host, channel, message) (":" + USER(nick, user, host) + " PRIVMSG #" + channel + " :" + message + "\r\n");

// TOPIC
# define RPL_TOPIC(nick, user, host, channel, topic) (":" + USER(nick, user, host) + " TOPIC #" + channel + " :" + topic + "\r\n");

// MODE
# define ERR_UMODEUNKNOWNFLAG(nick) (":localhost 501 " + nick + " :Unknown MODE flag" + "\r\n");
# define RPL_MODEL(nick, user, host, channel, mode) (":" + USER(nick, user, host) + " MODE #" + channel + " " + mode + "\r\n");

class Response
{
	private:
		static std::string userPrefix( Client *client );
	public:
		~Response();
		static void broadcastChannel( Client *client, Channel *channel, const std::string &response );
		static void broadcastAll( const std::map<int, Client> &clients, const std::string &response );
		static void message( Client *client, const std::string &response );
		static void numericReply( Client *client, const std::string &code, const std::vector<std::string> &args, const std::string &response );

		static void RPL_WELCOME( Client *client );
		static void ERR_NEEDMOREPARAMS( Client *client, const std::string &command );

		// NICK numeric codes
		static void ERR_NONICKNAMEGIVEN( Client *client );
		static void ERR_ERRONEUSNICKNAME( Client *client, const std::string &nickname );
		static void ERR_NICKNAMEINUSE( Client *client, const std::string &nickname );
		static void RPL_NICK( Client *client, const std::map<int, Client> &clients, const std::string &nickname );
		static void ERR_ALREADYREGISTERED( Client *client );

		// CHANNEL numeric codes
		static void ERR_NOSUCHCHANNEL( Client *client, const std::string &channel );
		static void ERR_NOTONCHANNEL( Client *client, const std::string &channel );
		static void ERR_INVITEONLYCHAN( Client *client, const std::string &channel );
		static void ERR_CHANNELISFULL( Client *client, const std::string &channel );
		static void ERR_BADCHANNELKEY( Client *client, const std::string &channel );

		// PART numeric codes
		static void RPL_PART( Client *client, Channel *channel, const std::string &message );

		// JOIN numeric codes
		static void RPL_JOIN( Client *client, Channel *channel );
};

#endif