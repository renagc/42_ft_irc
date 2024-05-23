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
# include <csignal>
# include <cstring>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include <map>
# include <poll.h>

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

/*
	Steps of a Server:
		initialization			getaddrinfo()
		create a socket			socket()
		bind a socket			bind()
		listen on the socket	listen()
		accept connection		accept()
		send and receive data	send(), recv(), recvfrom(), sento()
		disconnect				close()
*/

//		  uint32_t htonl(uint32_t hostlong);
//        uint16_t htons(uint16_t hostshort);

//        uint32_t ntohl(uint32_t netlong);
//        uint16_t ntohs(uint16_t netshort);

class Client;

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


		int									_next_client_id;
		int									_next_channel_id;

		/* debug */
		void						debug( void );


	public:
		Server(std::string port, std::string pw);
		~Server();

		/* member functions */

		// steps to create a server are: create a socket, bind the socket, listen on the socket
		void							startListen( void );

		// poll functions
		void							clientConnection( void );
		void							authenticateChecker( Client *client );
	
		void							knownConnection( int id );
		void							handleDataSender( const std::string &msg, Client *sender );

		// client functions
		Client							*findClientByFd( int fd );
		void							clientDisconnect( Client *client );

		// channel functions
		void							createChannel( const std::string &name, Client *admin );
		Client							*getClient( const std::string &nickname );

		// command chooser
		void							chooseCommand( Client *client, const std::vector<std::string> &cmd );

		// command finder
		void							findCommand( Client *client, const std::vector<std::string> &cmd );

		// commands functions
		void							joinCommand( Client *client, const std::string &channel_name );

		// nick command
		void									nickCommand( Client *client, const std::string &nickname );
		std::map<int, Client>::iterator			findNick( const std::string &nick );

		void									userCommand( Client *client, const std::string &username );
};

#endif