#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <iomanip>
# include <cstdlib>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <cstdio>
# include <csignal>
# include <cstring>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include <map>
# include <poll.h>
# include <unistd.h>
# include <fcntl.h>

#include <errno.h>

/*		IRC libs		*/
# include "Client.hpp"
# include "Channel.hpp"
# include "Response.hpp"
# include "Utils.hpp"

# define PORT_MIN_VALUE 1023
# define PORT_MAX_VALUE 65535

# define DEBUG(str) std::cout << "debugging: " << str << std::endl;
# define LOG(str) std::cout << "logging: " << str << std::endl;

class Client;
class Parser;

class Server
{
	private:
		std::string							_port;
		struct addrinfo						*_servinfo;
		int									_sockfd;
		std::vector<pollfd>					_pfds;
		std::map<int, Client>				_clients;
		std::map<std::string, Channel>		_channels;
		std::vector<std::string>			_commands;
		Parser								*_parsing;
		std::string							_password;

		/* debug */
		void						debug( void );

	public:
		Server(std::string port, std::string pw);
		~Server();

		/* member functions */

		// steps to create a server are: create a socket, bind the socket, listen on the socket
		void									startListen( void );

		// poll functions
		void									clientConnection( void );
	
		void									knownConnection( int id );
		void									handleDataSender( const std::string &msg, Client *sender );

		// client functions
		Client									*findClientByFd( int fd );
		void									clientDisconnect( Client *client );

		// channel functions
		void									createChannel( const std::string &name, Client *admin );
		Client									*getClient( const std::string &nickname );

		// get clients
		std::map<int, Client>					&getClients( void );
		std::map<std::string, Channel>			&getChannels( void );

		// get current id
		int										getNextClientId( void );
		int										getNextChannelId( void );
		const std::string						&getPassword( void ) const;
};

# include "Parser.hpp"

#endif