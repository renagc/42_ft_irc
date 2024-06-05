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

class Response
{
	public:
		static std::string userPrefix( Client *client );
		~Response();
		static void broadcastChannel( Client *client, Channel *channel, const std::string &response );
		static void broadcastAll( const std::map<int, Client> &clients, const std::string &response );
		static void ircMessage( Client *client, const std::string &response );
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
		static void RPL_TOPIC( Client *client, const std::string &channel, const std::string &topic );
		static void RPL_NAMREPLY( Client *client, const std::string &channel, const std::string &users );
		static void ERR_BANNEDFROMCHAN( Client *client, const std::string &channel );

		// QUIT
		static void RPL_QUIT( Client *client, Channel *channel, const std::string &message );

		// KICK
		static void ERR_CHANOPRIVSNEEDED( Client *client, const std::string &channel );
		static void RPL_KICK( Client *client, Channel *channel, const std::string &nickname );
		static void ERR_USERNOTINCHANNEL( Client *client, const std::string &channel, const std::string &nickname );

		// MODE
		static void ERR_UMODEUNKNOWNFLAG( Client *client );
		static void RPL_CHANNELMODEIS( Client *client, Channel *channel );
		static void ERR_KEYSET( Client *client, const std::string &channel );
		static void ERR_UNKNOWNMODE( Client *client, char c, const std::string &channel );

		// INVITE
		static void ERR_NOSUCHNICK( Client *client, const std::string &nickname );
		static void ERR_USERONCHANNEL( Client *client, const std::string &nickname, const std::string &channel );
		static void RPL_INVITING( Client *client, const std::string &nickname, const std::string &channel );

		// PRIVMSG
		static void ERR_CANNOTSENDTOCHAN( Client *client, const std::string &channel );

		// UNKNOWN COMMAND
		static void ERR_UNKNOWNCOMMAND( Client *client, const std::string &command );

		static void ERR_PASSWDMISMATCH( Client *client );
};

#endif