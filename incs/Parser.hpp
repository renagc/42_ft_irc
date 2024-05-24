#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <cctype>
# include <cstdlib>
# include <exception>
# include <map>
# include <utility>

// irc libs
# include "Response.hpp"
# include "Server.hpp"

class Parser
{
	private:
		Server								*_server;
		std::map<int, Client>				*_clients;
		std::map<std::string, Channel>		*_channels;

	public:
		// Parser( void );
		Parser( Server *server );
		~Parser();

		void								chooseParsing( Client *client, std::vector<std::string> cmd );
		std::map<int, Client>::iterator		findNick( const std::string &nick );
		void								authenticateChecker( Client *client );

		// commands functions
		void								nickCommand( Client *client, const std::string &nickname );

		// user command
		void								userCommand( Client *client, const std::string &username );

		// join command
		void								joinCommand( Client *client, const std::string &channel_name );

		// privmsg command
		void								privmsgCommand( Client *client, const std::string &channel_name, const std::string &message );

		// kick command
		void								kickCommand( Client *client, const std::string &channel_name, const std::string &username );
};

#endif // PARSER_HPP